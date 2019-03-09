#include <string>
#include <iostream>

#include "Handler.h"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << " No file path provided..." << std::endl;
		return -1;
	}
	std::string configFilePath = argv[1];

	if (argc != 2)
	{
		std::cout << " Too many arguments provided..." << std::endl;
	}
	else if (configFilePath.find(".conf") == std::string::npos)
	{
		std::cout << " ERROR: Config file is the wrong file type!" << std::endl;
		exit(EXIT_FAILURE);
	}

	Handler handler;
	handler.run(configFilePath);
	// printf("hello world\n");


	return 0;
}
