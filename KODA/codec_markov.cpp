#ifndef CODEC_MARKOV_H
#define CODEC_MARKOV_H

#include "codec_markov.h"

using namespace std;

Result<CompressionStats> compress_markov(const unsigned char* inDataBuffer, unsigned int bufferSize, ostream& out)
{
	// STUB: (brak kompresji)

	for(unsigned int i = 0; i < bufferSize; ++i)
		out.put(inDataBuffer[i]);

	CompressionStats stats;
	stats.entropy = 3.4;
	stats.meanBitLength = 3.65;
	stats.headerSize = 23;
	return{ true, stats };
}

bool decompress_markov(istream& in, unsigned char* outDataBuffer, unsigned int bufferSize)
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