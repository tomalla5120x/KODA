#ifndef CODEC_BLOCK_H
#define CODEC_BLOCK_H

#include "codec_block.h"
#include "tree.h"
#include "compression.h"
#include <bitset>

using namespace std;
using namespace Utils;

#define MAX_SYMBOL_INDEX 65535

Result<CompressionStats> compress_block(const unsigned char* inDataBuffer, unsigned int bufferSize, ostream& out)
{
	HuffmanTreeBuilder newTree(MAX_SYMBOL_INDEX);

	unsigned int doublesymbol;
	vector<unsigned int> inDataBufferDoublesymbols;

	for (unsigned int i = 0; i < bufferSize; ++i){
	doublesymbol = inDataBuffer[i] << 8;
	doublesymbol = doublesymbol | inDataBuffer[++i];
	newTree.addSymbol(doublesymbol);
	inDataBufferDoublesymbols.push_back(doublesymbol);
	}

	vector<unsigned int> bitLengths;
	newTree.getBitLengths(bitLengths);

	for (std::vector<unsigned int>::iterator it = bitLengths.begin(); it != bitLengths.end(); ++it) {
		uint16Write(out, *it);
	}


	HuffmanTreeEncoder encoder(bitLengths);
	BitStreamManager bitStreamManager(out);
	for (unsigned int i = 0; i < inDataBufferDoublesymbols.size(); ++i) {
		Code code = encoder.getCode((unsigned int)inDataBufferDoublesymbols[i]);
		bitStreamManager.addCode(code);
	}
	bitStreamManager.finish();


	CompressionStats stats;

	double meanBitLengths = 0.0;
	for (unsigned int i = 0; i < inDataBufferDoublesymbols.size(); ++i) {
		meanBitLengths += bitLengths[inDataBufferDoublesymbols[i]];
	}
	meanBitLengths = meanBitLengths / inDataBufferDoublesymbols.size();
	double entropy = newTree.getEntropy() / 2;

	stats.entropy = entropy;
	stats.meanBitLength = meanBitLengths;
	stats.headerSize = 131072;
	return{ true, stats };
}

bool decompress_block(istream& in, unsigned char* outDataBuffer, unsigned int bufferSize)
{
	vector<unsigned int> bitLengths;

	unsigned char inBytes[2];
	for (unsigned int i = 0; i < 65536; ++i) {
		unsigned char byte = in.get();
		unsigned char byte2 = in.get();
		if (byte == EOF || byte2 == EOF) {
			cerr << "Unexpected end of file." << endl;
			return false;
		}
		inBytes[0]= byte;
		inBytes[1]= byte2;
		bitLengths.push_back(uint16Read(inBytes));
	}

	HuffmanTreeDecoder decoder(bitLengths);
	int i = 0;
	auto node = decoder.getRoot();
	while (in) {
		unsigned char byte = in.get();
		for (int j = 7; j >= 0; --j) {
			if (((byte >> j) & 0x1) == 0x1) {
				node = node->traverseOne();
			}
			else {
				node = node->traverseZero();
			}
			if (node->isLeaf()) {
				int symbol = node->getSymbol();
				unsigned char symbolFirst = symbol >> 8;
				unsigned char symbolSecond = symbol & 255;
				outDataBuffer[i] = symbolFirst;
				outDataBuffer[++i] = symbolSecond;
				if (i == bufferSize) {
					break;
				}
				i++;
				node = decoder.getRoot();
			}
		}
		if (byte == EOF) {
			cerr << "Unexpected end of file." << endl;
			return false;
		}
	}

	return true;
}

#endif