#ifndef CODEC_H
#define CODEC_H

struct CompressionStats
{
	// entropia modelu wykorzystana podczas kompresji
	double entropy;
	// œrednia d³ugoœæ s³owa kodowego w bitach
	double meanBitLength;
	// wielkoœæ nag³ówka zawieraj¹cego metadane o kompresji, np. drzewa Huffmana
	unsigned int headerSize;
};

#endif