#include <iostream>
#include <fstream>
#include <string>

#include "codec_dispatch.h"

using namespace std;

bool run(int argc, char** argv)
{
	ConfigurationManager configManager(cout, cerr);
	Configuration config;
	if(!configManager.handleOptions(argc, argv, config))
		return false;

	if(config.isHelp())
		return true;

	// -------------

	ifstream fInput;
	ofstream fOutput;

	fInput.open(config.getInputPath(), ios::in | ios::binary);
	
	if(!fInput.good())
	{
		cerr << "Could not open file for reading: " << config.getInputPath() << endl;
		return false;
	}

	fOutput.open(config.getOutputPath(), ios::out | ios::binary | ios::trunc);

	if(!fOutput.good())
	{
		cerr << "Could not open file for writing: " << config.getOutputPath() << endl;
		return false;
	}

	return dispatch(fInput, fOutput, config);
}

int main(int argc, char** argv)
{
	// przyk³adowe argumenty wywo³ania do testowania
	//char* arguments[] = { argv[0], "-i", "../data/natural/barbara.pgm", "-o", "compressed.dat", "-c", "1"};
	//run(7, arguments);

	if(run(argc, argv))
		return EXIT_SUCCESS;
	return EXIT_FAILURE;
}