#ifndef CODEC_CLASSIC_H
#define CODEC_CLASSIC_H

#include "codec_classic.h"

using namespace std;

Result<CompressionStats> compress_classic(const unsigned char* inDataBuffer, unsigned int bufferSize, ostream& out)
{
	// STUB: (brak kompresji)

	for(unsigned int i = 0; i < bufferSize; ++i)
		out.put(inDataBuffer[i]);

	CompressionStats stats;
	stats.entropy = 3.4; // przyk³adowa wartoœæ
	stats.meanBitLength = 3.65; // przyk³adowa wartoœæ
	stats.headerSize = 0;

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