#ifndef CODEC_BLOCK_H
#define CODEC_BLOCK_H

#include <iostream>
#include "codec.h"
#include "utils.h"

Result<CompressionStats> compress_block(const unsigned char* inDataBuffer, unsigned int bufferSize, std::ostream& out);
bool decompress_block(std::istream& in, unsigned char* outDataBuffer, unsigned int bufferSize);

#endif