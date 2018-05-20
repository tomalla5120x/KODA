#include "image_utils.h"

#include <opencv2/imgcodecs.hpp>
#include <vector>
#include <memory>

using namespace std;
using namespace cv;

namespace ImageUtils
{
	bool pixelsWrite(unsigned char* pixelData, unsigned int width, unsigned int height, ostream& out)
	{
		vector<int> compressionParams;

		compressionParams.push_back(IMWRITE_PXM_BINARY);
		compressionParams.push_back(1);

		Mat pixelDataMat(height, width, CV_8UC1, pixelData);
		vector<unsigned char> encodedImage;

		if(!imencode(".pgm", pixelDataMat, encodedImage, compressionParams))
			return false;

		out.write((const char*)&encodedImage[0], encodedImage.size());

		return true;
	}

	Mat pixelsRead(istream& in)
	{
		in.seekg(0, ios_base::end);
		unsigned int inputSize = (unsigned int)in.tellg();

		unique_ptr<unsigned char[]> buffer = make_unique<unsigned char[]>(inputSize);
		in.seekg(0);
		in.read((char*)buffer.get(), inputSize);

		Mat rawData(1, inputSize, CV_8UC1, (void*)buffer.get());
		return imdecode(rawData, IMREAD_GRAYSCALE);
	}
}
