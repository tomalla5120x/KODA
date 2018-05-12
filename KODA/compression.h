#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <iostream>
#include <vector>
#include "tree.h"

using namespace std;

void compressByUsedBits(const unsigned char* inDataBuffer, unsigned int bufferSize, HuffmanTreeEncoder& encoder, ostream& out);

#endif