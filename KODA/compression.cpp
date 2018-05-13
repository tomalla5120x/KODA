#include "compression.h"

using namespace std;

BitStreamManager::BitStreamManager(ostream& out) : out(out), freeBits(8), currChar('\0') {}

void BitStreamManager::addCode(Code code){
	int bitsToSave = 8;
	
	for (unsigned int j = 0; j < code.bytes.size(); ++j) {	//petla po wszystkich bajtach danego s³owa kodowego
		unsigned char currCode = code.bytes[j];
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
	}
}

void BitStreamManager::finish() {
	if (freeBits < 8)
		out.put(currChar);
}