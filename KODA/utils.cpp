#include <string>
#include <sstream>

#include "utils.h"

// TK TODO: skasowaæ niepotrzebne funkcje

using namespace std;

NullStream cnull;

namespace Utils
{
	bool str2int(const string& str, int& out)
	{
		try {
			out = stoi(str);
		}
		catch(...) {
			return false;
		}

		return true;
	}

	bool str2double(const string& str, double& out)
	{
		try {
			out = stod(str);
		}
		catch(...) {
			return false;
		}

		return true;
	}

	bool csv2vector(const string& str, vector<int>& out)
	{
		stringstream ss;
		ss << str;

		while(!ss.eof())
		{
			string str;
			getline(ss, str, ',');

			int val;
			if(!str2int(str, val))
				return false;
			out.push_back(val);
		}

		return true;
	}

	bool nextLine(istream& in, string& outLine)
	{
		return nextToken(in, outLine, '\n');
	}

	bool nextLine(istream& in, stringstream& outLine)
	{
		string strLine;
		if(!nextToken(in, strLine, '\n'))
			return false;

		// reset the outLine contents
		outLine.str(string());
		outLine.clear();
		outLine << strLine;
		return true;
	}

	bool nextToken(istream& in, string& outString, char separator)
	{
		if(in.eof())
			return false;
		getline(in, outString, separator);
		return true;
	}

	void uint32Write(ostream& out, uint32_t value)
	{
		out.put(value & 0xFF);
		out.put((value >> 8) & 0xFF);
		out.put((value >> 16) & 0xFF);
		out.put(value >> 24);
	}

	uint32_t uint32Read(unsigned char* buffer)
	{
		uint32_t n = 0x0;

		n ^= buffer[3];
		n <<= 8;
		n ^= buffer[2];
		n <<= 8;
		n ^= buffer[1];
		n <<= 8;
		n ^= buffer[0];

		return n;
	}

	void uint16Write(ostream& out, uint16_t value)
	{
		out.put(value & 0xFF);
		out.put(value >> 8);
	}

	uint16_t uint16Read(unsigned char* buffer)
	{
		uint16_t n = 0x0;

		n ^= buffer[1];
		n <<= 8;
		n ^= buffer[0];

		return n;
	}
}