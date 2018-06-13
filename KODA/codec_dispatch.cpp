#include <memory>
#include <fstream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "codec_dispatch.h"
#include "utils.h"
#include "image_utils.h"

#include "codec_classic.h"
#include "codec_block.h"
#include "codec_markov.h"

using namespace cv;
using namespace std;

#define HEADER_COMPRESS_MAGIC1 0x50 // P
#define HEADER_COMPRESS_MAGIC2 0x26 // &

#define HEADER_COMPRESS_CLASSIC 0x01
#define HEADER_COMPRESS_BLOCK   0x02
#define HEADER_COMPRESS_MARKOV  0x03

void outputHistogram(istream& in, ostream& out, bool isBinary)
{
	unsigned int buckets[256] = { 0 };

	Mat pixelDataMat = ImageUtils::pixelsRead(in);
	const unsigned char* pixelData = pixelDataMat.ptr();
	unsigned int pixelsCount = pixelDataMat.rows * pixelDataMat.cols;

	for(unsigned int i = 0; i < pixelsCount; ++i)
		++buckets[pixelData[i]];

	if(!isBinary)
	{
		for(unsigned int i = 0; i < 256; ++i)
			out << buckets[i] << endl;
	}
	else
	{
		unsigned int maxCount = 0;

		for(unsigned int i = 0; i < 256; ++i)
			if(buckets[i] > maxCount)
				maxCount = buckets[i];

		Mat histMat = Mat::ones(256, 256, CV_8U) * 255;

		for(unsigned int i = 0; i < 256; ++i)
			line(histMat, Point(i, 255 - (256 * buckets[i] / maxCount)), Point(i, 255), Scalar(100, 100, 100), 1, LINE_8);

		ImageUtils::pixelsWrite(histMat.ptr(), histMat.cols, histMat.rows, out);
	}
}

bool dispatch_compress(istream& in, ostream& out, Configuration& config)
{
	if(config.getCompressionType() == CompressionType::NONE)
	{
		cerr << "No compression type specified." << endl;
		return false;
	}

	Mat decodedImage = ImageUtils::pixelsRead(in);

	if(decodedImage.empty())
	{
		cerr << "Input image is corrupted." << endl;
		return false;
	}

	if(decodedImage.depth() != CV_8U)
	{
		cerr << "Image has incorrect bit depth (unsigned 8 bit expected)." << endl;
		return false;
	}

	//#define CV_8U   0
	//#define CV_8S   1
	//#define CV_16U  2
	//#define CV_16S  3
	//#define CV_32S  4
	//#define CV_32F  5
	//#define CV_64F  6

	// ---------------------

	out.put(HEADER_COMPRESS_MAGIC1);
	out.put(HEADER_COMPRESS_MAGIC2);

	Utils::uint32Write(out, decodedImage.cols);
	Utils::uint32Write(out, decodedImage.rows);
	unsigned int pixelsCount = decodedImage.cols*decodedImage.rows;
	Result<CompressionStats> result;

	unsigned int compressionOffset = (unsigned int)out.tellp() + 1; // an additional byte for the compression method

	if(config.getCompressionType() == CompressionType::CLASSIC)
	{
		out.put(HEADER_COMPRESS_CLASSIC);
		result = compress_classic(decodedImage.ptr(), pixelsCount, out);
	}
	else if(config.getCompressionType() == CompressionType::BLOCK)
	{
		out.put(HEADER_COMPRESS_BLOCK);
		result = compress_block(decodedImage.ptr(), pixelsCount, out);
	}
	else if(config.getCompressionType() == CompressionType::MARKOV)
	{
		out.put(HEADER_COMPRESS_MARKOV);
		result = compress_markov(decodedImage.ptr(), pixelsCount, out);
	} else
	{
		cerr << "Unknown compression type." << endl;
		return false;
	}

	if(!result.success)
		return false;

	out.seekp(0, ios_base::end);

	in.seekg(0, ios_base::end);
	unsigned int inputSize = (unsigned int)in.tellg();

	cout << "Model entropy: " << result.result.entropy << endl;
	cout << "Mean bit length: " << result.result.meanBitLength << endl;
	cout << "Compressed file size efficiency: " << (1.0 * inputSize / out.tellp()) << endl;
	cout << "Compressed data efficiency: " << (1.0 * pixelsCount / ((unsigned int)out.tellp() - compressionOffset - result.result.headerSize)) << endl;
	cout << "Compressed file space savings(cp): " << (1.0 - 1.0 * out.tellp() / inputSize) * 100 << endl;
	cout << "Compressed data space savings(cp): " << (1.0 - 1.0 * ((unsigned int)out.tellp() - compressionOffset - result.result.headerSize) / pixelsCount) * 100 << endl;

	// ----- histogram -------

	if(config.isHistogramPath())
	{
		ofstream fOutputHist;

		if(config.isHistogramBinary())
			fOutputHist.open(config.getHistogramPath(), ios::out | ios::binary | ios::trunc);
		else
			fOutputHist.open(config.getHistogramPath(), ios::out | ios::trunc);

		if(!fOutputHist.good())
		{
			cerr << "Could not open file for writing: " << config.getHistogramPath() << endl;
			return false;
		}

		outputHistogram(in, fOutputHist, config.isHistogramBinary());
	}

	return true;
}

bool dispatch_decompress(istream& in, ostream& out, Configuration& config)
{
	in.seekg(2);

	unsigned char buffer[4];
	in.read((char*)buffer, 4);
	unsigned int width = Utils::uint32Read(buffer);
	in.read((char*)buffer, 4);
	unsigned int height = Utils::uint32Read(buffer);

	unsigned int pixels = width*height;

	unsigned char compressionType;
	in.read((char*)(&compressionType), 1);

	unique_ptr<unsigned char[]> pixelData = make_unique<unsigned char[]>(pixels);

	bool decompressionResult = false;

	if(compressionType == HEADER_COMPRESS_CLASSIC)
		decompressionResult = decompress_classic(in, pixelData.get(), pixels);
	else if(compressionType == HEADER_COMPRESS_BLOCK)
		decompressionResult = decompress_block(in, pixelData.get(), pixels);
	else if(compressionType == HEADER_COMPRESS_MARKOV)
		decompressionResult = decompress_markov(in, pixelData.get(), pixels);
	else
	{
		cerr << "Unknown compression type" << endl;
		return false;
	}

	if(!decompressionResult)
		return false;

	if(!ImageUtils::pixelsWrite(pixelData.get(), width, height, out))
	{
		cerr << "Error encoding output image." << endl;
		return false;
	}

	return true;
}

bool dispatch(istream& in, ostream& out, Configuration& config)
{
	unsigned char magic[2];
	in.read((char*)&magic, 2);

	if(magic[0] == 0x50 && magic[1] == 0x35) // 'P5' oznacza binarny format PGM
		return dispatch_compress(in, out, config);
	else if(magic[0] == HEADER_COMPRESS_MAGIC1 && magic[1] == HEADER_COMPRESS_MAGIC2)
		return dispatch_decompress(in, out, config);
	
	cerr << "Unknown file format encountered. Expected binary PGM or compressed binary PGM." << endl;
	return false;
	
}