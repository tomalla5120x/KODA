#ifndef CODEC_CLASSIC_H
#define CODEC_CLASSIC_H

#include "codec_classic.h"
#include "tree.h"
using namespace std;

#define SYMBOLS_COUNT 255


void saveHeader(vector<unsigned int>& bitLengths, ostream& out) {
	for (std::vector<unsigned int>::iterator it = bitLengths.begin(); it != bitLengths.end(); ++it) {
		unsigned int intvalue = *it;
		vector< unsigned char> bitLenghtCharBuffor(1);
		memcpy(&bitLenghtCharBuffor[0], &intvalue, 1);
		unsigned char  bitLenghtChar = bitLenghtCharBuffor[0];
		out.put(bitLenghtChar);
		bitLenghtCharBuffor.clear();
	}
}

Result<CompressionStats> compress_classic(const unsigned char* inDataBuffer, unsigned int bufferSize, ostream& out)
{
	vector<unsigned int> bitLengths;
	vector<unsigned char> outBites;

	HuffmanTreeBuilder newTree(SYMBOLS_COUNT);
	for (unsigned int i = 0; i < bufferSize; ++i) {	
		newTree.addSymbol(inDataBuffer[i]);
	}
	newTree.getBitLengths(bitLengths);
	saveHeader(bitLengths, out);
	HuffmanTreeEncoder encoder(bitLengths);
	compressByUsedBits(inDataBuffer, bufferSize, encoder, out);
	
	CompressionStats stats;
	stats.entropy = newTree.getEntropy();
	double meanBitLengths = 0.0;
	for (unsigned int i = 0; i < bufferSize; ++i) {
		meanBitLengths += bitLengths[inDataBuffer[i]];
	}
	stats.meanBitLength = meanBitLengths / bufferSize;
	stats.headerSize = SYMBOLS_COUNT;

	return {true, stats};
}


bool decompress_classic(istream& in, unsigned char* outDataBuffer, unsigned int bufferSize)
{
	// STUB: (brak kompresji)

	for(unsigned int i = 0; i < bufferSize; ++i)
	{
		unsigned char byte = in.get();

		if(byte == EOF)
		{
			cerr << "Unexpected end of file." << endl;
			return false;
		}

		outDataBuffer[i] = byte;
	}
		
	return true;
}

#endif