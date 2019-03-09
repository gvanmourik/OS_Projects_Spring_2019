#ifndef HANDLER_H
#define HANDLER_H

#include <string>
#include <vector>
#include <stdio.h>

#include "MetaIO.h"
#include "ConfigIO.h"


class Handler 
{
private:


public:

	bool run(std::string configPath)
	{
		std::vector<std::string> errorLog;

		/// CONFIG
		ConfigIO config_handler(configPath);
		
		if ( !config_handler.readFile(errorLog) )
		{
			printErrors(errorLog, 
						config_handler.getLogPath(),
						config_handler.getLogType());
			
			std::cout << " --> exited with config handler error" << std::endl;
			exit(EXIT_FAILURE);
		}

		// collect the meta-data file path
		std::string metaDataPath;
		if ( !config_handler.getMetaPath(metaDataPath) )
			return false;


		/// META
		MetaIO meta_handler("src/" + metaDataPath, 
							config_handler.getSystemMemory(), 
							config_handler.getRuntimeKey());

		// std::cout <<"before readfile"<<std::endl;
		if ( !meta_handler.readFile(errorLog) )
		{
			// print errors
			printErrors(errorLog,
						config_handler.getLogPath(),
						config_handler.getLogType());
			
			std::cout << " --> exited with meta handler error" << std::endl;
			exit(EXIT_FAILURE);
		}

		return true;
	}

	void printErrors(std::vector<std::string> &errors, std::string logFilePath, std::string logType)
	{
		// printf("hello\n");
		// std::cout << logType << std::endl;
		for (auto error : errors)
		{
			if ( logType == MONITOR )
			{
				std::cout << error;
			}
			if ( logType == BOTH || logType == FILE)
			{
				// printf("both\n");
				if ( logType == BOTH )
				{
					std::cout << error;
				}

				// std::cout << "HELLO" << std::endl;
				// std::cout << logFilePath << std::endl;
				std::ofstream logFile(logFilePath);
				logFile << error;
			}
		}
	}
};


#endif /* HANDLER_H */
