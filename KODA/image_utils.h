#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <iostream>
#include <opencv2/core/mat.hpp>

namespace ImageUtils
{
	bool pixelsWrite(unsigned char* pixelData, unsigned int width, unsigned int height, std::ostream& out);
	cv::Mat pixelsRead(std::istream& in);
}

#endif