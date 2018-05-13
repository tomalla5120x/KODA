#ifndef CODEC_CLASSIC_H
#define CODEC_CLASSIC_H

#include "codec_classic.h"
#include "tree.h"
using namespace std;

#define MAX_SYMBOL_INDEX 255

Result<CompressionStats> compress_classic(const unsigned char* inDataBuffer, unsigned int bufferSize, ostream& out) {
	vector<unsigned int> bitLengths;

	HuffmanTreeBuilder newTree(MAX_SYMBOL_INDEX);
	for (unsigned int i = 0; i < bufferSize; ++i) {	
		newTree.addSymbol(inDataBuffer[i]);
	}
	newTree.getBitLengths(bitLengths);
	for (std::vector<unsigned int>::iterator it = bitLengths.begin(); it != bitLengths.end(); ++it) {
		out.put(*it);
	}
	HuffmanTreeEncoder encoder(bitLengths);
	BitStreamManager bitStreamManager(out);
	for (unsigned int i = 0; i < bufferSize; ++i) {
		Code code = encoder.getCode((unsigned int)inDataBuffer[i]);
		bitStreamManager.addCode(code);
	}
	bitStreamManager.finish();
	
	CompressionStats stats;
	stats.entropy = newTree.getEntropy();
	double meanBitLengths = 0.0;
	for (unsigned int i = 0; i < bufferSize; ++i) {
		meanBitLengths += bitLengths[inDataBuffer[i]];
	}
	stats.meanBitLength = meanBitLengths / bufferSize;
	cout << stats.meanBitLength << endl;
	stats.headerSize = MAX_SYMBOL_INDEX + 1;

	return {true, stats};
}

bool decompress_classic(istream& in, unsigned char* outDataBuffer, unsigned int bufferSize) {
	
	vector<unsigned int> bitLengths;

	for (unsigned int i = 0; i < MAX_SYMBOL_INDEX + 1; ++i) {
		unsigned char byte = in.get();
		if (byte == EOF) {
			cerr << "Unexpected end of file." << endl;
			return false;
		}
		bitLengths.push_back((unsigned int)byte);
	}

	HuffmanTreeDecoder decoder(bitLengths);
	int i = 0;
	auto node = decoder.getRoot();
	while(in) {
		unsigned char byte = in.get();
		for (int j = 7; j >= 0; --j) {
			if (((byte >> j) & 0x1) == 0x1) {
				node = node->traverseOne();
			}
			else {
				node = node->traverseZero();
			}
			if (node->isLeaf()){
				int symbol = node->getSymbol();
				outDataBuffer[i] = (char) symbol;
				if (i == bufferSize) {
					break;
				}
				i++;
				node = decoder.getRoot();
			}
		}
		if(byte == EOF) {
			cerr << "Unexpected end of file." << endl;
			return false;
		}
	}	
	return true;
}

#endif