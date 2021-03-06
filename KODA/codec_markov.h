#ifndef CODEC_MARKOV_H
#define CODEC_MARKOV_H

#include <iostream>
#include "codec.h"
#include "utils.h"
#include "bits_managers.h"

Result<CompressionStats> compress_markov(const unsigned char* inDataBuffer, unsigned int bufferSize, std::ostream& out);
bool decompress_markov(std::istream& in, unsigned char* outDataBuffer, unsigned int bufferSize);

#endif