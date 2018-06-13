#include "codec_block.h"
#include "tree.h"
#include "bits_managers.h"
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
	if (bufferSize % 2 == 0 || (bufferSize % 2 == 1 && i < bufferSize - 1)) {
		doublesymbol = doublesymbol | inDataBuffer[++i];
	}
	newTree.addSymbol(doublesymbol);
	inDataBufferDoublesymbols.push_back(doublesymbol);
	}

	vector<unsigned int> bitLengths;
	newTree.getBitLengths(bitLengths);

	for (std::vector<unsigned int>::iterator it = bitLengths.begin(); it != bitLengths.end(); ++it) {
		uint16Write(out, *it);
	}


	HuffmanTreeEncoder encoder(bitLengths);
	BitStreamWriter bitStreamManager(out);
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
	meanBitLengths = meanBitLengths / bufferSize;
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
	for (unsigned int i = 0; i <= MAX_SYMBOL_INDEX; ++i) {
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
	BitStreamReader bitStreamReader(in);

	int i = 0;
	auto node = decoder.getRoot();
	while (in) {
		if (i == bufferSize) {
			break;
		}
		try {
			unsigned int symbol = bitStreamReader.nextSymbol(decoder);
			unsigned char symbolFirst = symbol >> 8;
			outDataBuffer[i] = symbolFirst;
			if (bufferSize % 2 == 0 || (bufferSize % 2 == 1 && i < bufferSize - 1)) {
				unsigned char symbolSecond = symbol & 255;
				outDataBuffer[++i] = symbolSecond;
			}
			i++;
		}
		catch (runtime_error& e) {
			cerr << e.what() << endl;
			return false;
		}
	}
	return true;
}