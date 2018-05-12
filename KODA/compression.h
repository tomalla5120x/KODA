#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <iostream>
#include <vector>
#include "tree.h"

using namespace std;

class BitStreamManager
{
public:
	BitStreamManager(ostream& out);
	void addCode(Code code);
	void finish();

private:
	ostream& out;
	int freeBits = 8;
	char currChar = '\0';
};

#endif