#ifndef CONFIG_IO
#define CONFIG_IO

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "ConfigData.h"

#define UNASSIGNED "NAH"

class ConfigIO 
{
private:
	double configVer;
	std::string FilePath;
	std::string logType;
	std::string logFilePath;
	std::string metaFilePath;
	// std::ostringstream std::cout;
	runtime_key_t RuntimeKey;
	std::vector<ConfigData> FileData;


public:
	/// Constructors
	ConfigIO(std::string _filePath, std::string _logFilePath = "NONE") : 
		FilePath(_filePath), logFilePath(_logFilePath)
	{
		logType = MONITOR;
		metaFilePath = UNASSIGNED;
	}
	~ConfigIO(){}


	// Access functions
	std::string getPath() { return FilePath; }
	std::string getLogType() { return logType; }
	std::string getLogPath() { return logFilePath; }
	// std::vector<ConfigData> getFileData() { return FileData; }
	runtime_key_t getRuntimeKey() { return RuntimeKey; }
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
	bool readFile(std::vector<std::string> &errlog)
	{
		std::string currentStr, fileExt;
		//check if config file is empty
		std::ifstream configFile(FilePath);
		if (configFile.peek() == std::ifstream::traits_type::eof())
		{
			errlog.push_back(" ERROR: Config file is empty!\n");
			return false;
		}

		if ( configFile.is_open() )
		{
			// check for start...throw error if "Start" not found
			std::getline(configFile, currentStr);
			if ( currentStr != "Start Simulator Configuration File" )
			{
				errlog.push_back(" ERROR: Config file not formatted correctly! {in start section}\n");
				return false;
			}

			// read in version number
			std::getline(configFile, currentStr, ':');
			if ( currentStr != "Version/Phase" )
			{
				errlog.push_back(" ERROR: Config file not formatted correctly! {version header}\n");
				return false;
			}
			configFile >> std::skipws >> currentStr;
			configVer = std::atof( currentStr.c_str() );

			// read in meta file path
			std::getline(configFile, currentStr); //skip newline
			std::getline(configFile, currentStr, ':');
			if ( currentStr != "File Path" )
			{
				errlog.push_back(" ERROR: Config file not formatted correctly! {in meta file path header}\n");
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
				errlog.push_back(" ERROR: Meta-data file is the wrong type!\n");
				return false;
			}
			std::getline(configFile, currentStr); //skip newline

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
					errlog.push_back(" ERROR: Config file not formatted correctly! {in config data}\n");
					return false;
				}
				// print each line of data in real-time
				// lineData.print();
				// add to file data
				FileData.push_back(lineData);
				
				// recover descriptor
				auto Descriptor = lineData.getDescriptor();
				// clean up descriptor string
				std::transform(Descriptor.begin(), Descriptor.end(), Descriptor.begin(), ::tolower);
				Descriptor.erase( remove_if(Descriptor.begin(), Descriptor.end(), isspace), Descriptor.end() );
				// add descriptor to key
				RuntimeKey[Descriptor] = lineData.getRuntime();
				
				std::getline(configFile, currentStr); //next line
				lineStart = currentStr.substr( 0, currentStr.find(":") );
				endOfConfigData = !std::any_of( lineStart.begin(), lineStart.end(), 
					[](char c){ return c == '{'; } );
			}

			// Collect log file and path
			lineStart = currentStr.substr(0, currentStr.find(":"));
			if ( lineStart != "Log" )
			{
				errlog.push_back(" ERROR: Config file not formatted correctly! {in log header}\n");
				return false;
			}
			// std::cout << lineStart << std::endl;

			logType = currentStr.substr( currentStr.find(":")+1 );
			logType.erase( remove_if(logType.begin(), logType.end(), isspace), logType.end() );
			// std::cout << "logType = " << logType << std::endl;
			if ( LogType.find(logType) == LogType.end() )
			{
				errlog.push_back(" ERROR: Log type is not allowed!\n");
				return false;
			}

			// std::cout << logType << std::endl;
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
				errlog.push_back(" ERROR: Log-data file is the wrong type!\n");
				return false;
			}
			
			// check end section of config file
			std::getline(configFile, currentStr); //skip newline
			std::getline(configFile, currentStr);
			if ( currentStr != "End Simulator Configuration File" )
			{
				errlog.push_back(" ERROR: Config file not formatted correctly! {in end section}\n");
				return false;
			}

			// check for end...throw error if "End" not fund
			configFile.close();
		}
		else
		{
			errlog.push_back(" ERROR: File did not open!\n");
			return false;
		}
		return true;
	}

	void print()
	{
		std::cout << "Configuration File Data:" << std::endl;
		for (auto lineData : FileData)
		{
			lineData.print();
		}

		std::cout << "\tLogged to: ";
		if ( logType == MONITOR )
		{
			std::cout << "monitor" << std::endl;
		}
		if ( logType == FILE )
		{
			std::cout << logFilePath << std::endl;
		}
		if ( logType == BOTH )
		{
			std::cout << "monitor and " << logFilePath << std::endl;
		}
		std::cout << std::endl;
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