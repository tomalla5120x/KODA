#include <limits>

#include "configuration.h"
#include "utils.h"

using namespace std;

ConfigurationManager::ConfigurationManager(): ConfigurationManager(cnull, cnull) {}
ConfigurationManager::ConfigurationManager(ostream& mainOut, ostream& errOut) : m_mainOut(mainOut), m_errOut(errOut) {}

bool ConfigurationManager::assertValue(string param)
{
	if(m_arguments.empty())
	{
		m_errOut << "Parameter " << param << " requires a value" << endl;
		return false;
	}

	return true;
}

bool ConfigurationManager::getString(std::string& out, std::string param)
{
	if(!assertValue(param))
		return false;

	out = m_arguments.front();
	m_arguments.pop();
	return true;
}

bool ConfigurationManager::getInt(int& out, std::string param)
{
	if(!assertValue(param))
		return false;

	string value = m_arguments.front();

	if(!Utils::str2int(value, out))
	{
		m_errOut << "Couldn't convert the following to an integer value: " << value << endl;
		return false;
	}

	m_arguments.pop();
	return true;
}

bool ConfigurationManager::getDouble(double& out, std::string param)
{
	if(!assertValue(param))
		return false;

	string value = m_arguments.front();

	if(!Utils::str2double(value, out))
	{
		m_errOut << "Couldn't convert the following to a floating-point value: " << value << endl;
		return false;
	}

	m_arguments.pop();
	return true;
}

bool ConfigurationManager::handleOptions(int argc, char** argv, Configuration& config)
{
	// clearing old arguments
	while(!m_arguments.empty())
		m_arguments.pop();

	for(int i = 1; i < argc; ++i)
		m_arguments.push(string(argv[i]));

	if(m_arguments.empty())
	{
		m_errOut << "No parameters specified." << endl;
		listHelp(m_errOut);
		return false;
	}

	while(!m_arguments.empty())
	{
		if(!handleOption(config))
		{
			listHelp(m_errOut);
			return false;
		} else if(config.isHelp())
		{
			// przed chwil¹ zosta³ w³¹czony tryb "help" - ignorujemy kolejne parametry
			return true;
		}
	}

	// ------ parametry obowi¹zkowe ---------

	if(config.getInputPath().empty())
	{
		m_errOut << "No input file specified." << endl;
		return false;
	}

	if(config.getOutputPath().empty())
	{
		m_errOut << "No output file specified." << endl;
		return false;
	}

	return true;
}

bool ConfigurationManager::handleOption(Configuration& config)
{
	if(m_arguments.empty())
		return false;

	string param = m_arguments.front();
	m_arguments.pop();

	if(param[0] != '-')
	{
		m_errOut << "Expected a parameter, but a value token found: " << param << endl;
		return false;
	}

	// ---------------

	if(param == "-h" || param == "--help")
	{
		config.m_isHelp = true;
		listHelp(m_mainOut);
		return true;
	}

	if(param == "-b" || param == "--histogrambinary")
	{
		config.m_isHistogramBinary = true;
		return true;
	}

	if(param == "-g" || param == "--histogram")
		return getString(config.m_histogramPath, param);

	if(param == "-i" || param == "--input")
		return getString(config.m_inputPath, param);

	if(param == "-o" || param == "--output")
		return getString(config.m_outputPath, param);

	if(param == "-c" || param == "--compression")
	{
		int compressionTypeCode;
		if(!getInt(compressionTypeCode, param))
			return false;

		if(compressionTypeCode == 1)
			config.m_compressionType = CompressionType::CLASSIC;
		else if(compressionTypeCode == 2)
			config.m_compressionType = CompressionType::BLOCK;
		else if(compressionTypeCode == 3)
			config.m_compressionType = CompressionType::MARKOV;
		else
		{
			m_errOut << "Incorrect compression type specified. Found: " << compressionTypeCode << endl;
			return false;
		}

		return true;
	}

	m_errOut << "Unknown parameter: " << param << endl;
	return false;
}

void ConfigurationManager::listHelp(ostream& out)
{
	out << endl;
	out << "Parameters: " << endl;
	out << endl;
	out << "\t-h, --help" << endl;
	out << "\t\tShow this help screen." << endl;
	out << endl;
	out << "\t-g, --histogram = path" << endl;
	out << "\t\tThe path to the file where the histogram of the original image will be output." << endl;
	out << "\t\tDefault: no histogram is generated." << endl;
	out << endl;
	out << "\t-b, --histogrambinary" << endl;
	out << "\t\tThe flag indicating that the histogram should be an image." << endl;
	out << "\t\tDefault: histogram is a list of occurences of each pixel value in plain text form." << endl;
	out << endl;
	out << "\t-i, --input = path" << endl;
	out << "\t\tThe path to the input file (PGM file or compressed PGM file)." << endl;
	out << "\t\tThis parameter is mandatory." << endl;
	out << endl;
	out << "\t-o, --output = path" << endl;
	out << "\t\tThe path to the output file." << endl;
	out << "\t\tThis parameter is mandatory." << endl;
	out << endl;
	out << "\t-c or --compression = algorithm" << endl;
	out << "\t\tThe algorithm to be used for compression." << endl;
	out << "\t\tPossible values:" << endl;
	out << "\t\t\t1 - the classing Huffman encoding" << endl;
	out << "\t\t\t2 - the blocking Huffman encoding (two input symbols per one output symbol)" << endl;
	out << "\t\t\t3 - the Huffman encoding for Markov chain model" << endl;
	out << "\t\tThis parameter is mandatory when compressing the input file." << endl;
}

