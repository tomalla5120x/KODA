#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <ostream>
#include <vector>

class NullBuffer: public std::streambuf
{
public:
	int overflow(int c) { return c; }
};

class NullStream: public std::ostream
{
public:
	NullStream(): std::ostream(&m_sb) {}
private:
	NullBuffer m_sb;
};

extern NullStream cnull;

// ----------------

template<typename T>
struct Result
{
	bool success;
	T result;
};

namespace Utils
{
	bool str2int(const std::string& str, int& out);
	bool str2double(const std::string& str, double& out);

	bool csv2vector(const std::string& str, std::vector<int>& out);

	bool nextLine(std::istream& in, std::string& outLine);
	bool nextLine(std::istream& in, std::stringstream& outLine);
	bool nextToken(std::istream& in, std::string& outString, char separator);

	// outputs and inputs in little endian
	void uint32Write(std::ostream& out, uint32_t value);
	uint32_t uint32Read(unsigned char* buffer);
	void uint16Write(std::ostream& out, uint16_t value);
	uint16_t uint16Read(unsigned char* buffer);
}

#endif