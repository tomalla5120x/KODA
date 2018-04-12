#include "tree.h"

#include <queue>
#include <cmath>

using namespace std;

void outputBits(ostream& out, unsigned char bits, unsigned char bitsCount = 8)
{
	for(int i = 0; i < bitsCount; ++i)
	{
		if(bits & 0x80)
			out << '1';
		else
			out << '0';
		bits <<= 1;
	}
}

ostream& operator<<(ostream& out, const Code& code)
{
	for(int i = 0; i < code.bytes.size() - 1; ++i)
	{
		outputBits(out, code.bytes[i]);
		out << ' ';
	}

	outputBits(out, code.bytes[code.bytes.size() - 1], code.lastByteBitsUsed);
	return out;
}

void createHuffmanTable(const vector<unsigned int>& inBitLengths, vector<HuffmanTableEntry>& outEntries)
{
	outEntries.clear();
	outEntries.resize(inBitLengths.size());

	for(unsigned int i = 0; i<(unsigned int)inBitLengths.size(); ++i)
	{
		HuffmanTableEntry& entry = outEntries[i];
		entry.symbolIndex = i;
		entry.bitLength = inBitLengths[i];

		unsigned int bytesCount = (entry.bitLength - 1) / 8 + 1;
		unsigned int bitsLeft = (entry.bitLength - 1) % 8 + 1;

		entry.code.bytes.resize(bytesCount, 0);
		entry.code.lastByteBitsUsed = bitsLeft;
	}

	sort(outEntries.begin(), outEntries.end(),
		[](const HuffmanTableEntry& a, const HuffmanTableEntry& b) -> bool
	{
		if(a.bitLength == b.bitLength)
			return a.symbolIndex < b.symbolIndex;
		return a.bitLength < b.bitLength;
	});

	for(unsigned int i = 1; i<(unsigned int)outEntries.size(); ++i)
	{
		HuffmanTableEntry& previous = outEntries[i - 1];
		HuffmanTableEntry& current = outEntries[i];

		unsigned int lastBitLength = previous.bitLength;
		
		for(unsigned int k = 0; k < (unsigned int)previous.code.bytes.size(); ++k)
			current.code.bytes[k] = previous.code.bytes[k];

		// arithmetic incrementation
		unsigned int lastByteIndex = (unsigned int)previous.code.bytes.size() - 1;
		current.code.bytes[lastByteIndex] >>= (8 - previous.code.lastByteBitsUsed);
		++current.code.bytes[lastByteIndex];
		current.code.bytes[lastByteIndex] <<= (8 - previous.code.lastByteBitsUsed);

		if(current.code.bytes[lastByteIndex] == 0) // overflow
			for(int k = lastByteIndex-1;; --k)
			{
				++current.code.bytes[k];
				if(current.code.bytes[k] != 0)
					break;

				if(k<0)
				{
					cerr << "Incorrect bit lengths." << endl;
					throw runtime_error("Incorrect bit lengths.");
				}
			}
	}
}

HuffmanTreeEncoder::HuffmanTreeEncoder(const vector<unsigned int>& bitLengths)
{
	createHuffmanTable(bitLengths, m_codes);

	sort(m_codes.begin(), m_codes.end(),
		[](const HuffmanTableEntry& a, const HuffmanTableEntry& b) -> bool
	{
		return a.symbolIndex < b.symbolIndex;
	});
}

Code HuffmanTreeEncoder::getCode(unsigned int symbolIndex) const
{
	return m_codes[symbolIndex].code;
}

HuffmanTreeDecoder::Node* HuffmanTreeDecoder::createNode(HuffmanTableEntry* pFirst, HuffmanTableEntry* pLast, unsigned int currentByte, unsigned int currentBit)
{
	if(pFirst == pLast)
		return new Node(pFirst->symbolIndex);

	if(currentBit == 8)
	{
		currentBit = 0;
		++currentByte;
	}

	HuffmanTableEntry* current = pFirst;

	for(; current != pLast; ++current)
		if(current->code.bytes[currentByte] & (1 << (7 - currentBit)))
			break;

	return new Node(createNode(pFirst, current-1, currentByte, currentBit + 1), createNode(current, pLast, currentByte, currentBit + 1));
}

HuffmanTreeDecoder::HuffmanTreeDecoder(const vector<unsigned int>& vBitLengths)
{
	vector<HuffmanTableEntry> vEntries;
	createHuffmanTable(vBitLengths, vEntries);

	m_pRoot.reset(createNode(&vEntries[0], &vEntries[vEntries.size()-1], 0, 0));
}

HuffmanTreeBuilder::HuffmanTreeBuilder(unsigned int maxSymbolIndex): m_weights(maxSymbolIndex + 1, 0), m_totalSymbols(0) {}

void HuffmanTreeBuilder::addSymbol(unsigned int symbolIndex)
{
	if(symbolIndex >= m_weights.size())
		throw std::invalid_argument("Given index is out of range of the alphabet.");

	++m_weights[symbolIndex];
	++m_totalSymbols;
}

void HuffmanTreeBuilder::getBitLengths(vector<unsigned int>& outBitLengths) const
{
	struct GroupEntry
	{
		unsigned int totalWeight;
		vector<unsigned int> symbols;
	};

	struct GroupEntryComparator
	{
		bool operator()(GroupEntry const &l, GroupEntry const &r) {
			return l.totalWeight > r.totalWeight;
		}
	};

	outBitLengths.clear();
	outBitLengths.resize(m_weights.size(), 1);

	priority_queue<GroupEntry, vector<GroupEntry>, GroupEntryComparator> qGroups;

	for(size_t i = 0; i < m_weights.size(); ++i)
	{
		GroupEntry entry;
		entry.totalWeight = m_weights[i];
		entry.symbols.push_back((unsigned int)i);

		qGroups.push(move(entry));
	}

	while(qGroups.size() > 2)
	{
		GroupEntry entry1 = qGroups.top();
		qGroups.pop();
		GroupEntry entry2 = qGroups.top();
		qGroups.pop();

		if(entry1.symbols.size() < entry2.symbols.size())
			swap(entry1, entry2);

		entry1.totalWeight += entry2.totalWeight;
		entry1.symbols.reserve(entry1.symbols.size() + entry2.symbols.size());
		entry1.symbols.insert(entry1.symbols.begin(), entry2.symbols.begin(), entry2.symbols.end());

		for(auto it = entry1.symbols.begin(); it != entry1.symbols.end(); ++it)
			++outBitLengths[*it];

		qGroups.push(entry1);
	}
}

double HuffmanTreeBuilder::getEntropy() const
{
	double entropy = 0.0;

	for(auto it = m_weights.begin(); it != m_weights.end(); ++it)
	{
		double p = 1.0 * (*it) / m_totalSymbols;
		entropy -= p * log2(p);
	}

	return entropy;
}