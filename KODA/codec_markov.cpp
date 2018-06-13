#include "codec_markov.h"
#include "tree.h"
#include <deque>
using namespace std;

#define MAX_SYMBOL_INDEX 255
#define START_CONTEXT 0

Result<CompressionStats> compress_markov(const unsigned char* inDataBuffer, unsigned int bufferSize, ostream& out)
{
	vector<vector<unsigned int>> bitLengths(256);
	vector<unsigned int> symbolCount(256, 0);
	deque<HuffmanTreeEncoder> encoder;
	deque<HuffmanTreeBuilder> tree;
	unsigned char currentSymbol;
	unsigned char nextSymbol;
	unsigned int sumBitLen;

	for (unsigned int i = 0; i < 256; ++i) {
		tree.emplace_back(MAX_SYMBOL_INDEX);
	}

	tree[START_CONTEXT].addSymbol(inDataBuffer[0]);
	for (unsigned int i = 0; i < bufferSize - 1; ++i) {
		currentSymbol = inDataBuffer[i];
		nextSymbol = inDataBuffer[i + 1];
		tree[currentSymbol].addSymbol(nextSymbol);
	}

	for (unsigned int i = 0; i < 256; ++i) {
		tree[i].getBitLengths(bitLengths[i]);
		for (unsigned int j = 0; j < 256; ++j) {
			out.put(bitLengths[i][j]);
		}
		encoder.emplace_back(bitLengths[i]);
	}
	
	BitStreamWriter bitStreamManager(out);
	Code startCode = encoder[START_CONTEXT].getCode((unsigned int)inDataBuffer[0]);
	bitStreamManager.addCode(startCode);
	sumBitLen = bitLengths[START_CONTEXT][inDataBuffer[0]];
	for (unsigned int i = 0; i < bufferSize - 1; ++i) {
		currentSymbol = inDataBuffer[i];
		nextSymbol = inDataBuffer[i + 1];
		Code code = encoder[currentSymbol].getCode((unsigned int)nextSymbol);
		bitStreamManager.addCode(code);
		symbolCount[currentSymbol]++;
		sumBitLen += bitLengths[currentSymbol][nextSymbol];
	}
	symbolCount[nextSymbol]++;
	bitStreamManager.finish();

	CompressionStats stats;
	stats.entropy = 0.0;

	for (unsigned int i = 0; i < 256; ++i) {
		stats.entropy += 1.0 * symbolCount[i] / bufferSize * tree[i].getEntropy();
	}
	
	stats.meanBitLength = 1.0 * sumBitLen / bufferSize;
	stats.headerSize = 65536;
	return{ true, stats };
}

bool decompress_markov(istream& in, unsigned char* outDataBuffer, unsigned int bufferSize)
{
	vector<vector<unsigned int>> bitLengths(256);
	deque<HuffmanTreeDecoder> decoder;

	for (unsigned i = 0; i < 256; ++i) {
		for (unsigned int j = 0; j < 256; ++j) {
			unsigned char byte = in.get();
			if (byte == EOF) {
				cerr << "Unexpected end of file." << endl;
				return false;
			}
			bitLengths[i].push_back((unsigned int)byte);
		}
		decoder.emplace_back(bitLengths[i]);
	}

	BitStreamReader bitStreamReader(in);
	unsigned char currentSymbol = START_CONTEXT;
	for (unsigned int i = 0; i < bufferSize; ++i) {
		try {
			currentSymbol = (unsigned char)bitStreamReader.nextSymbol(decoder[currentSymbol]);
			outDataBuffer[i] = currentSymbol;
		}
		catch (runtime_error& e){
			cerr << e.what() << endl;
			return false;
		}
	}

	return true;
}