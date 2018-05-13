#include "compression.h"

using namespace std;

BitStreamManager::BitStreamManager(ostream& out) : out(out), freeBits(8), currChar('\0') {}

void BitStreamManager::addCode(Code code){
	int bitsToSave = 8;
	
	for (unsigned int j = 0; j < code.bytes.size(); ++j) {	//petla po wszystkich bajtach danego s�owa kodowego
		unsigned char currCode = code.bytes[j];
		if (j == (code.bytes.size() - 1)) {		//warunek dla ostatnigo bajtu, dla ktorego mo�e by� brana pod uwage tylko cz�� bit�w  
			bitsToSave = code.lastByteBitsUsed;
			currCode = currCode >> (8 - bitsToSave);
			currCode = currCode << (8 - bitsToSave);
		}
		currChar = currChar | (currCode >> (8 - freeBits));
		if (bitsToSave > freeBits) {		//liczba bit�w do zapisania przekracza dost�pn� liczb� miejsc w aktualnie wype�nianym bajcie
			out.put(currChar);
			currChar = currCode >> (8 - bitsToSave);
			freeBits += 8 - bitsToSave;
			currChar = currChar << freeBits;
		}
		else if (bitsToSave == freeBits){	//liczba bit�w do zapisania jest r�wna liczbie dost�pnych miejsc w aktualnie wype�nianym bajcie
			out.put(currChar);
			currChar = '\0';
			freeBits = 8;
		}
		else {		//liczba bit�w do zapisania jest mniejsza od liczby dost�pnych miejsc w aktualnie wype�nianym bajcie
			freeBits -= bitsToSave;
		}
	}
}

void BitStreamManager::finish() {
	if (freeBits < 8)
		out.put(currChar);
}