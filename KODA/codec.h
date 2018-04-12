#ifndef CODEC_H
#define CODEC_H

struct CompressionStats
{
	// entropia modelu wykorzystana podczas kompresji
	double entropy;
	// �rednia d�ugo�� s�owa kodowego w bitach
	double meanBitLength;
	// wielko�� nag��wka zawieraj�cego metadane o kompresji, np. drzewa Huffmana
	unsigned int headerSize;
};

#endif