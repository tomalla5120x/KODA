#include <iostream>
#include <fstream>
#include <string>

#include "codec_dispatch.h"
#include "image_utils.h"

#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

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
	} else
	{
		unsigned int maxCount = 0;

		for(unsigned int i = 0; i < 256; ++i)
			if(buckets[i] > maxCount)
				maxCount = buckets[i];

		Mat histMat = Mat::ones(256, 256, CV_8U)*255;

		for(unsigned int i = 0; i < 256; ++i)
			line(histMat, Point(i, 255 - (256 * buckets[i] / maxCount)), Point(i, 255), Scalar(100, 100, 100), 1, LINE_8);

		ImageUtils::pixelsWrite(histMat.ptr(), histMat.cols, histMat.rows, out);
	}
}

bool run(int argc, char** argv)
{
	ConfigurationManager configManager(cout, cerr);
	Configuration config;
	if(!configManager.handleOptions(argc, argv, config))
		return false;

	if(config.isHelp())
		return true;

	// -------------

	ifstream fInput;
	ofstream fOutput;

	fInput.open(config.getInputPath(), ios::in | ios::binary);
	
	if(!fInput.good())
	{
		cerr << "Could not open file for reading: " << config.getInputPath() << endl;
		return false;
	}

	fOutput.open(config.getOutputPath(), ios::out | ios::binary | ios::trunc);

	if(!fOutput.good())
	{
		cerr << "Could not open file for writing: " << config.getOutputPath() << endl;
		return false;
	}

	if(!dispatch(fInput, fOutput, config))
		return false;

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

		outputHistogram(fInput, fOutputHist, config.isHistogramBinary());
	}

	return true;
}

int main(int argc, char** argv)
{
	// przyk³adowe argumenty wywo³ania do testowania
	//char* arguments[] = { argv[0], "-i", "../data/natural/barbara.pgm", "-o", "compressed.dat", "-c", "1"};
	//run(7, arguments);

	if(run(argc, argv))
		return EXIT_SUCCESS;
	return EXIT_FAILURE;
}