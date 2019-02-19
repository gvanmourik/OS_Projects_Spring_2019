#ifndef FILE_IO
#define FILE_IO

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <type_traits>

#include "MetaData.h"
#include "ConfigData.h"

enum file_t {CONFIG, METADATA};

template<class T>
class FileIO 
{
private:
	file_t fileType;
	std::string filePath;
	std::string logFilePath;
	std::vector<T> FileData; //vector of either meta or config data


public:
	/// Constructors
	FileIO(std::string _filePath, std::string _logFilePath) : 
		filePath(_filePath), logFilePath(_logFilePath)
	{
		if (std::is_same<T, ConfigData>::value) { fileType = CONFIG; }
		if (std::is_same<T, MetaData>::value) { fileType = METADATA; }
	}
	~FileIO(){}

	/// Public functions
	// Access functions
	file_t getType() { return fileType; }
	std::string getPath() { return filePath; }
	std::string getLogPath() { return logFilePath; }
	
	// I/O functions
	bool readFile()
	{
		// if config
		if ( fileType == CONFIG )
		{
			printf("in config type...\n");

			std::ifstream configFile(filePath);
			if ( configFile.is_open() )
			{
				// check for start...throw error if "Start" not found
				printf("config file is open...\n");

				// read in version number
			    // read in file path
				// iteratively read in ConfigData until "Log:"
					// read while first word does not have ":"
				// read in log file name
				// read in log file path
				// check for end...throwo error if "End" not fund
			}
			else
			{
				printf("config file is NOT open...\n");

				//throw file not open error
				return false;
			}
			
			configFile.close();
		}


		// if meta
		if ( fileType == METADATA )
		{
			printf("in meta type...\n");
		}

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

/// Example meta-data file:
	// 1 Start Program Meta-Data Code:
	// 2 S{begin}0; A{begin}0; P{run}11; P{run}9; P{run}12;
	// 3 P{run}9; P{run}11; P({run}8; P{run}14; P{run}14; P{run}12;
	// 4 P{run}12; P{run}6; P{run}8; P{run}9; P{run}6; P{run}14;
	// 5 P{run}15; P{run}12; P{run}9; P{run}6; P{run}5; A{finish}0;
	// 6 A{begin}0; P{run}6; P{run}6; P{run}9; P{run}11; P{run}13;
	// 7 P{run}14; P{run}5; P{run}7; P{run}14; P{run}15; P{run}7;
	// 8 P{run}5; P{run}14; P{run}15; P{run}14; P{run}7; P{run}14;
	// 9 P{run}13; P{run}8; P{run}7; A{finish}0; A{begin}0; P{run}6;
	// 10 P{run}10; P{run}13; P{run}9; P{run}15; P{run}6; P{run}13;
	// 11 P{run}11; P{run}5; P{run}6; P{run}7; P{run}12; P{run}11;
	// 12 P{run}6; P{run}8; P{run}10; P{run}5; P{run}8; P{run}9; P{run}7;
	// 13 A{finish}0; S{finish}0.
	// 14 End Program Meta-Data Code.

#endif /* FILE_IO */