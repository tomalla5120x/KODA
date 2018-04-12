#include <memory>

#include <opencv2/imgcodecs.hpp>

#include "codec_dispatch.h"
#include "utils.h"

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

bool dispatch_compress(istream& in, ostream& out, Configuration& config)
{
	if(config.getCompressionType() == CompressionType::NONE)
	{
		cerr << "No compression type specified." << endl;
		return false;
	}

	in.seekg(0, ios_base::end);
	unsigned int inputSize = (unsigned int)in.tellg();

	unique_ptr<unsigned char[]> buffer = make_unique<unsigned char[]>(inputSize);
	in.seekg(0);
	in.read((char*)buffer.get(), inputSize);

	Mat rawData(1, inputSize, CV_8UC1, (void*)buffer.get());
	Mat decodedImage = imdecode(rawData, IMREAD_GRAYSCALE);

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

	cout << "Model entropy: " << result.result.entropy << endl;
	cout << "Mean bit length: " << result.result.meanBitLength << endl;
	cout << "Compressed file size efficiency: " << (1.0 * inputSize / out.tellp()) << endl;
	cout << "Compressed data efficiency: " << (1.0 * pixelsCount / ((unsigned int)out.tellp() - compressionOffset - result.result.headerSize)) << endl;

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

	vector<int> compressionParams;

	compressionParams.push_back(IMWRITE_PXM_BINARY);
	compressionParams.push_back(1);

	Mat pixelDataMat(height, width, CV_8UC1, pixelData.get());
	vector<unsigned char> encodedImage;

	if(!imencode(".pgm", pixelDataMat, encodedImage, compressionParams))
	{
		cerr << "Error encoding output image." << endl;
		return false;
	}
	
	out.write((const char*)&encodedImage[0], encodedImage.size());

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