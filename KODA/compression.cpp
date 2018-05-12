#include "compression.h"

using namespace std;


void printBinary(char c) {
	for (int i = 7; i >= 0; --i) {
		cout << ((c & (1 << i)) ? '1' : '0');
	}
	cout << endl;
}


void compressByUsedBits(const unsigned char* inDataBuffer, unsigned int bufferSize, HuffmanTreeEncoder& encoder, ostream& out) {
	
	int freeBits = 8;
	char currChar = '\0';
	
	for (unsigned int i = 0; i < bufferSize; ++i) {		//pêtla po wszystkich elementach buforu wejsciowego
		Code code = encoder.getCode((unsigned int)inDataBuffer[i]);
		int bitsToSave = 8;

		for (unsigned int j = 0; j < code.bytes.size(); ++j) {	//petla po wszystkich bajtach danego s³owa kodowego
			char currCode = code.bytes[j];
			if (j == (code.bytes.size() - 1)) {		//warunek dla ostatnigo bajtu, dla ktorego mo¿e byæ brana pod uwage tylko czêœæ bitów  
				bitsToSave = code.lastByteBitsUsed;
				currCode = currCode >> (8 - bitsToSave);
				currCode = currCode << (8 - bitsToSave);
			}
			currChar = currChar | (currCode >> (8 - freeBits));	
			if (bitsToSave > freeBits) {		//liczba bitów do zapisania przekracza dostêpn¹ liczbê miejsc w aktualnie wype³nianym bajcie
				out.put(currChar);
				currChar = currCode >> (8 - bitsToSave);
				freeBits += 8 - bitsToSave;
				currChar = currChar << freeBits;
			}
			else if (bitsToSave == freeBits){	//liczba bitów do zapisania jest równa liczbie dostêpnych miejsc w aktualnie wype³nianym bajcie
				out.put(currChar);
				currChar = '\0';
				freeBits = 8;
			}
			else {		//liczba bitów do zapisania jest mniejsza od liczby dostêpnych miejsc w aktualnie wype³nianym bajcie
				freeBits -= bitsToSave;
			}
			//dla ostatniego bajtu ze danych wejsciowych i dla ostatniego bajtu ze struktury Code zapisywany jest aktualnie wype³niany bajt
			if (i == (bufferSize - 1) && j == (code.bytes.size() - 1) && freeBits < 8) {
				out.put(currChar);
			}
		}
	}
}