#include <iostream>
#include "codec.h"
#include "utils.h"
#include "bits_managers.h"

Result<CompressionStats> compress_classic(const unsigned char* inDataBuffer, unsigned int bufferSize, std::ostream& out);
bool decompress_classic(std::istream& in, unsigned char* outDataBuffer, unsigned int bufferSize);