#ifndef BITS_MANAGERS_H
#define BITS_MANAGERS_H

#include <iostream>
#include <vector>
#include "tree.h"

using namespace std;

class BitStreamWriter
{
public:
	BitStreamWriter(ostream& out);
	void addCode(Code code);
	void finish();

private:
	ostream& out;
	int freeBits = 8;
	char currChar = '\0';
};


class BitStreamReader
{
public:
	BitStreamReader(istream& in);
	unsigned int nextSymbol(HuffmanTreeDecoder& decoder);

private:
	istream& in;
	int bitsLeft = 0;
	unsigned char currByte = '\0';
};

#endif