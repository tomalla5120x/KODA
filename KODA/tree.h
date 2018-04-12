#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>
#include <memory>

// struktura zawieraj¹ca pojedyncze s³owo kodowe w postaci listy bajtów
// np. dla bajtów: 11010011 10100000 i lastBytebitsUsed=3, wychodzi
// nastêpuj¹ce s³owo kodowe: 11010011101
struct Code
{
	std::vector<unsigned char> bytes;
	unsigned char lastByteBitsUsed;
};

std::ostream& operator<< (std::ostream& out, const Code& code);

struct HuffmanTableEntry
{
	unsigned int symbolIndex;
	unsigned int bitLength;
	Code code;
};

class HuffmanTreeEncoder
{
public:
	HuffmanTreeEncoder(const std::vector<unsigned int>& bitLengths);
	HuffmanTreeEncoder(const HuffmanTreeEncoder&) = delete;
	Code getCode(unsigned int symbolIndex) const;

private:
	std::vector<HuffmanTableEntry> m_codes;
};

class HuffmanTreeDecoder
{
	class Node
	{
	public:
		bool isLeaf() const {
			return m_pNodeZero.get() == nullptr;
		}

		unsigned int getSymbol() const {
			return m_symbol;
		}

		const Node* traverseZero() const {
			return m_pNodeZero.get();
		}

		const Node* traverseOne() const {
			return m_pNodeOne.get();
		}

	private:
		friend HuffmanTreeDecoder;

		Node(unsigned int symbol): m_pNodeZero(nullptr), m_pNodeOne(nullptr), m_symbol(symbol) {}
		Node(Node* pZero, Node* pOne): m_pNodeZero(pZero), m_pNodeOne(pOne), m_symbol(0) {}
		Node(const Node&) = delete;
		void operator=(const Node&) = delete;

		std::unique_ptr<Node> m_pNodeZero;
		std::unique_ptr<Node> m_pNodeOne;

		unsigned int m_symbol;
	};

public:
	HuffmanTreeDecoder(const std::vector<unsigned int>& bitLengths);
	HuffmanTreeDecoder(const HuffmanTreeDecoder&) = delete;

	const Node* getRoot() const {
		return m_pRoot.get();
	}

private:
	Node* createNode(HuffmanTableEntry* pFirst, HuffmanTableEntry* pLast, unsigned int currentByte, unsigned int currentBit);

	std::unique_ptr<Node> m_pRoot;
};

class HuffmanTreeBuilder
{
public:
	// symbole alfabetu s¹ indeksowane od zera
	// je¿eli wiêc symboli jest 256, to maxSymbolIndex bêdzie równy 255
	HuffmanTreeBuilder(unsigned int maxSymbolIndex);
	HuffmanTreeBuilder(const HuffmanTreeBuilder&) = delete;
	virtual ~HuffmanTreeBuilder() {}

	// dodanie do tworzonego drzewa symbolu o wskazanym indeksie
	void addSymbol(unsigned int symbolIndex);
	// zwraca listê d³ugoœci bitowych s³ów kodowych kolejnych symboli w alfabecie
	void getBitLengths(std::vector<unsigned int>& outBitLengths) const;

	// zwraca wartoœæ entropii na podstawie zbudowanego modelu
	double getEntropy() const;

private:
	unsigned int m_totalSymbols;
	std::vector<unsigned int> m_weights;
};

#endif