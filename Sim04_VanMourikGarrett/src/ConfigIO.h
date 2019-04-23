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
	int sysMemory;
	int sysMemorySize;
	int quantumNumber;
	std::string scheduleType;
	std::string FilePath;
	std::string logType;
	std::string logFilePath;
	std::string metaFilePath;
	// std::ostringstream std::cout;
	runtime_key_t RuntimeKey;
	std::vector<ConfigData> FileData;
	resource_t Resources;


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
	int getSysMemory() { return sysMemory; }
	int getSysMemorySize() { return sysMemorySize; }
	int getQuantumNumber() { return quantumNumber; }
	std::string getScheduleType() { return scheduleType; }
	std::string getPath() { return FilePath; }
	std::string getLogType() { return logType; }
	std::string getLogPath() { return logFilePath; }
	resource_t getResources() { return Resources; }
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
			errlog.push_back(" ERROR: Config file is empty or does not exist!\n");
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


			// // Collect quantum number 
			// std::string quantumNumberStr;
			// std::getline(configFile, currentStr, ':');
			// if ( currentStr != "Processor Quantum Number")
			// {
			// 	errlog.push_back(" ERROR: Config file not formatted correctly! {processor quantum number}\n");
			// 	return false;
			// }
			// configFile >> std::skipws >> quantumNumberStr;
			// quantumNumber = std::atoi( quantumNumberStr.c_str() );
			// std::getline(configFile, currentStr); //skip newline


			// // Collect cpu scheduling mode
			// std::getline(configFile, currentStr, ':');
			// if ( currentStr != "CPU Scheduling Code")
			// {
			// 	errlog.push_back(" ERROR: Config file not formatted correctly! {CPU Scheduling Code}\n");
			// 	return false;
			// }
			// configFile >> std::skipws >> scheduleType;
			// std::getline(configFile, currentStr); //skip newline

			//allows backwards compatibility 
			std::vector<std::string> lines;
			std::vector<std::string> values;
			
			for (int i=0; i<2; ++i)
			{
				std::getline(configFile, currentStr, ':');
				lines.push_back(currentStr);
				configFile >> std::skipws >> currentStr;
				values.push_back(currentStr);
				std::getline(configFile, currentStr); //skip newline
			}

			if ( lines[0] == "Processor Quantum Number" && 
				 lines[1] == "CPU Scheduling Code" )
			{
				quantumNumber = std::atoi( values[0].c_str() );
				scheduleType = values[1];
			}
			else
			{
				std::cout << " -- WARNING: Either the quantum number or the CPU scheduling code were not specified! Defaulting to '1' and 'FIFO'." << std::endl;
				quantumNumber = 1;
				scheduleType = "FIFO";
			}


			// Collect meta data line-by-line
			ConfigData lineData;
			std::getline(configFile, currentStr);
			auto lineStart = currentStr.substr( 0, currentStr.find(":")+1 );
			// bool endOfConfigData = !std::any_of( lineStart.begin(), lineStart.end(), 
			// 	[](const char c){ return c == '{'; } );
			while ( lineStart != "Log:" )
			{
				//debug
				// std::cout << currentStr << std::endl;

				if ( !lineData.extractData(currentStr) )
				{
					errlog.push_back(" ERROR: Config file not formatted correctly! {in config data}\n");
					return false;
				}
				// print each line of data in real-time
				// lineData.print();
				// add to file data
				FileData.push_back(lineData);

				// set system memory
				if ( lineData.getDescriptor() == "System memory" )
				{
					sysMemory = lineData.getValue();
				}

				// set system memory block size
				if ( lineData.getDescriptor() == "Memory block size" )
				{
					sysMemorySize = lineData.getValue();
				}

				// get and clean up descriptor string (tolower and then remove white space)
				auto Descriptor = lineData.getDescriptor();
				std::transform(Descriptor.begin(), Descriptor.end(), Descriptor.begin(), ::tolower);
				Descriptor.erase( remove_if(Descriptor.begin(), Descriptor.end(), isspace), Descriptor.end() );

				// collect resources
				if ( lineData.isResource() )
				{
					//debug
					// std::cout << "descriptor = " << lineData.getDescriptor() << std::endl;
					
					std::vector<bool> flags(lineData.getValue());
					for ( auto flag : flags)
					{
						flag = AVAILABLE;
					}
					Resources[lineData.getDescriptor()] = flags;
				}
				else
				{
					RuntimeKey[Descriptor] = lineData.getValue();
				}

				// auto Descriptor = lineData.getDescriptor();
				// // clean up descriptor string (tolower and then remove white space)
				// std::transform(Descriptor.begin(), Descriptor.end(), Descriptor.begin(), ::tolower);
				// Descriptor.erase( remove_if(Descriptor.begin(), Descriptor.end(), isspace), Descriptor.end() );
				// add descriptor to key
				// RuntimeKey[Descriptor] = lineData.getValue();
				
				// std::getline(configFile, currentStr); //next line
				// lineStart = currentStr.substr( 0, currentStr.find(":") );
				// endOfConfigData = !std::any_of( lineStart.begin(), lineStart.end(), 
				// 	[](char c){ return c == '{'; } );
				ConfigData lineData;
				std::getline(configFile, currentStr);
				lineStart = currentStr.substr( 0, currentStr.find(":")+1 );
			}


			// Collect log file and path
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


#endif /* CONFIG_IO */