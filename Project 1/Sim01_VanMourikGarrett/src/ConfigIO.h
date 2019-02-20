#ifndef CONFIG_IO
#define CONFIG_IO

#include <vector>
#include <fstream>
#include <iostream>
#include <type_traits>

#include "MetaData.h"
#include "ConfigData.h"

#define UNASSIGNED "NAH"

enum file_t {CONFIG, METADATA};

class ConfigIO 
{
private:
	double configVer;
	std::string FilePath;
	std::string logFileName;
	std::string logFilePath;
	std::string metaFilePath;
	std::vector<ConfigData> FileData;


public:
	/// Constructors
	ConfigIO(std::string _filePath, std::string _logFilePath = "NONE") : 
		FilePath(_filePath), logFilePath(_logFilePath)
	{
		metaFilePath = UNASSIGNED;
	}
	~ConfigIO(){}


	// Access functions
	std::string getPath() { return FilePath; }
	std::string getLogPath() { return logFilePath; }
	bool getMetaPath(std::string &returnMetaPath) 
	{
		if ( metaFilePath != UNASSIGNED )
		{
			returnMetaPath = metaFilePath;
			return true;
		}
		return false;
	}
	
	// I/O functions
	bool readFile()
	{
		std::string currentStr, fileExt;
		std::ifstream configFile(FilePath);
		if ( configFile.is_open() )
		{
			// check for start...throw error if "Start" not found
			std::getline(configFile, currentStr);
			if ( currentStr != "Start Simulator Configuration File" )
			{
				//throw 'file not formatted correctly' error
				return false;
			}


			// read in version number
			std::getline(configFile, currentStr, ':');
			if ( currentStr != "Version/Phase" )
			{
				//throw 'file not formatted correctly' error
				return false;
			}
			configFile >> std::skipws >> currentStr;
			configVer = std::atof( currentStr.c_str() );
			printf("%f\n", configVer );


			// read in meta file path
			std::getline(configFile, currentStr); //skip newline
			std::getline(configFile, currentStr, ':');
			if ( currentStr != "File Path" )
			{
				//throw 'file not formatted correctly' error
				return false;
			}
			configFile >> std::skipws >> metaFilePath;


			// determine meta data file type
			auto fileExtStart = metaFilePath.end();
			while ( *fileExtStart != '.')
			{
				fileExtStart--;
			}
			for (auto it = fileExtStart; it != metaFilePath.end(); ++it)
			{
				fileExt += *it;
			}
			if ( fileExt != ".mdf" )
			{
				//throw 'provided meta data file is wrong type' error
				return false;
			}
			std::getline(configFile, currentStr); //skip newline
			printf("%s\n", fileExt.c_str() );


			// Collect meta data line-by-line
			ConfigData lineData;
			std::getline(configFile, currentStr);
			auto lineStart = currentStr.substr( 0, currentStr.find(":") );
			bool endOfConfigData = !std::any_of( lineStart.begin(), lineStart.end(), 
				[](const char c){ return c == '{'; } );
			while ( !endOfConfigData )
			{
				if ( !lineData.extractData(currentStr) )
				{
					printf("ERROR!!!\n");
					//throw 'file not formatted correctly' error
					return false;
				}
				FileData.push_back(lineData);
				
				std::getline(configFile, currentStr); //next line
				lineStart = currentStr.substr( 0, currentStr.find(":") );
				endOfConfigData = !std::any_of( lineStart.begin(), lineStart.end(), 
					[](char c){ return c == '{'; } );
			}


			// Collect log file and path
			logFileName = currentStr.substr( currentStr.find(":")+2 );
			std::getline(configFile, currentStr, ':');
			configFile >> std::skipws >> logFilePath;

			// determine log file type
			fileExt.clear();
			fileExtStart = logFilePath.end();
			while ( *fileExtStart != '.')
			{
				fileExtStart--;
			}
			for (auto it = fileExtStart; it != logFilePath.end(); ++it)
			{
				fileExt += *it;
			}
			if ( fileExt != ".lgf" )
			{
				//throw 'provided meta data file is wrong type' error
				return false;
			}
			std::getline(configFile, currentStr); //skip newline
			printf("%s\n", fileExt.c_str() );


			std::getline(configFile, currentStr);
			if ( currentStr != "End Simulator Configuration File" )
			{
				//throw 'file not formatted correctly' error
				return false;
			}



			// check for end...throw error if "End" not fund
			printf("closing config file...\n");
			configFile.close();
		}
		else
		{
			printf("config file is NOT open...\n");

			//throw 'file not open' error
			return false;
		}

		printf("done.......\n");

		return true;
	}


};


/// Example configuration file:
	// 1 Start Simulator Configuration File
	// 2 Version/Phase: 2.0
	// 3 File Path: Test_2e.mdf
	// 4 Projector cycle time {msec}: 25
	// 5 Processor cycle time {msec}: 10
	// 6 Keyboard cycle time {msec}: 50
	// 7 Monitor display time {msec}: 20
	// 8 Scanner cycle time {msec}: 10
	// 9 Hard drive cycle time {msec}: 15
	// 10 Log: Log to Both
	// 11 Log File Path: logfile_1.lgf
	// 12 End Simulator Configuration File


#endif /* CONFIG_IO */