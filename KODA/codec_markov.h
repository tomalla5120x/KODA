#include <iostream>
#include "codec.h"
#include "utils.h"

Result<CompressionStats> compress_markov(const unsigned char* inDataBuffer, unsigned int bufferSize, std::ostream& out);
bool decompress_markov(std::istream& in, unsigned char* outDataBuffer, unsigned int bufferSize);