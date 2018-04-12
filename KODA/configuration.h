#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <queue>
#include <ostream>

enum class CompressionType {
	NONE, // rodzaj kompresji nie zosta³ podany
	CLASSIC, // model bez pamiêci (model blokowy rzêdu 1)
	BLOCK,   // model bez pamiêci (model blokowy rzêdu 2)
	MARKOV   // model z pamiêci¹ (model Markowa rzêdu 1)
};

class Configuration
{
public:
	Configuration(): m_isHelp(false),
		m_histogramPath(""),
		m_outputPath(""),
		m_inputPath(""),
		m_compressionType(CompressionType::NONE) {}

	bool isHelp() const {
		return m_isHelp;
	}

	bool isHistogramPath() const {
		return !m_histogramPath.empty();
	}

	std::string getHistogramPath() const {
		return m_histogramPath;
	}

	std::string getInputPath() const {
		return m_inputPath;
	}

	std::string getOutputPath() const {
		return m_outputPath;
	}

	CompressionType getCompressionType() const {
		return m_compressionType;
	}

private:
	friend class ConfigurationManager;

	// czy program jest w trybie "help" i wypisuje manual
	bool m_isHelp;

	// œcie¿ka pod któr¹ zostanie zapisany histogram
	std::string m_histogramPath;

	// œcie¿ka pliku wejœciowego
	std::string m_inputPath;

	// œcie¿ka pliku wyjœciowego
	std::string m_outputPath;

	// zastosowany typ kompresji
	CompressionType m_compressionType;
};

class ConfigurationManager
{

public:
	ConfigurationManager();
	ConfigurationManager(std::ostream& mainOut, std::ostream& errOut);

	bool handleOptions(int argc, char** argv, Configuration& pOutConfiguration);

private:
	void listHelp(std::ostream& out);

	bool handleOption(Configuration& config);
	bool assertValue(std::string param);

	bool getString(std::string& out, std::string param);
	bool getInt(int& out, std::string param);
	bool getDouble(double& out, std::string param);

private:
	// aktualna kolejka argumentów
	std::queue<std::string> m_arguments;

	// wykorzystywany kiedy jest flaga wypisuj¹ca help - wówczas jest to g³ówny rezultat pracy programu
	std::ostream& m_mainOut;
	// wykorzystywany tylko dla b³êdów podczas parsowania parametrów
	std::ostream& m_errOut;
};

#endif