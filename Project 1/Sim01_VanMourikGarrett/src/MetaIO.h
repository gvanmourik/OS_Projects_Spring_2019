#ifndef META_IO
#define META_IO

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <type_traits>

#include "ConfigData.h"
#include "MetaData.h"

#define UNASSIGNED "NAH"

class MetaIO 
{
private:
	bool startFlag = false;
	std::string FilePath;
	std::vector<MetaData> FileData;


public:
	/// Constructors
	MetaIO(std::string _filePath) : FilePath(_filePath){}
	~MetaIO(){}


	// Access functions
	std::string getPath() { return FilePath; }
	

	// Other functions
	bool readFile(std::vector<std::string> &errlog)
	{
		// std::cout << "in read file..." << std::endl;

		std::string currentStr;
		std::ifstream metaFile(FilePath);
		if (metaFile.peek() == std::ifstream::traits_type::eof())
		{
			errlog.push_back(" ERROR: Meta-Data file is empty!\n");
			return false;
		}

		if ( metaFile.is_open() )
		{
			// std::cout << "file is open..." << std::endl;
			if ( !readline(metaFile, currentStr) )
			{
				errlog.push_back(" ERROR: Meta-data file not formatted correctly! {start/end section}\n");
				return false;
			}

			MetaData lineData;
			std::string dataStr;
			
			// Collect meta-data
			if ( !readline(metaFile, currentStr) )
			{
				errlog.push_back(" ERROR: Meta-data file not formatted correctly! {start/end section}\n");
				return false;
			}
			bool lastMetaDataLine;
			if ( *(currentStr.end()-1) == '.' )
				lastMetaDataLine = true;
			
			while ( !lastMetaDataLine )
			{
				// remove the whitespace from each line
				currentStr.erase( remove_if(currentStr.begin(), currentStr.end(), isspace), currentStr.end() );
				std::stringstream lineStream(currentStr);
			    while ( std::getline(lineStream, dataStr, ';') ) 
			    {
					if ( !lineData.extractData(dataStr) )
					{
						errlog.push_back(" ERROR: File not formatted correctly! {in meta-data}\n");
						return false;
					}
					// lineData->print();
					FileData.push_back(lineData);
			    }

			    if ( !readline(metaFile, currentStr) )
				{
					errlog.push_back(" ERROR: Meta-data file not formatted correctly! {start/end section}\n");
					return false;
				}
				if ( *(currentStr.end()-1) == '.' )
					lastMetaDataLine = true;
			}

			// deal with the last valid meta data line
			currentStr.erase( remove_if(currentStr.begin(), currentStr.end(), isspace), currentStr.end() );
			auto lastString = currentStr.substr(0, currentStr.find("."));
			std::stringstream lineStream(lastString);
			while ( std::getline(lineStream, dataStr, ';') ) 
		    {
		    	// remove whitespace from string
		        dataStr.erase( remove_if(dataStr.begin(), dataStr.end(), isspace), dataStr.end() );
		        // std::cout << dataStr << std::endl;

				if ( !lineData.extractData(dataStr) )
				{
					errlog.push_back(" ERROR: File not formatted correctly! {in meta-data}\n");
					return false;
				}
				FileData.push_back(lineData);
		    }

		    if ( !readline(metaFile, currentStr) )
			{
				errlog.push_back(" ERROR: Meta-data file not formatted correctly! {start/end section\n");
				return false;
			}
			if ( currentStr != "End Program Meta-Data Code." )
			{
				errlog.push_back(" ERROR: File not formatted correctly! {in start section}\n");
				return false;
			}
		    metaFile.close();
		}
		else
		{
			errlog.push_back(" ERROR: File did not open!\n");
			return false;
		}

		// std::cout << "done..." << std::endl;

		return true;
	}

	// Custom version of getline that both gets the line and checks if
	// 	that line is the end of the meta-data
	bool readline(std::ifstream &metaFile, std::string &currentStr)
	{
		std::getline(metaFile, currentStr);
		if ( currentStr == "Start Program Meta-Data Code:" )
		{
			// if flag = true; u saw another Start -> return false;
			if ( startFlag )
				return false;
			else
				startFlag = true;
		}
		else if ( currentStr == "End Program Meta-Data Code." )
		{
			if ( startFlag )
				startFlag = false;
			else
				return false;
		}
		return true;
	}

	void print(runtime_key_t RTKey)
	{
		std::cout << "Meta-Data Metrics:" << std::endl;
		for (auto metaLineData : FileData)
		{
			int runtime;
			if ( metaLineData.getType() == "S" )
			{
				metaLineData.print(0);
			}
			if ( metaLineData.getType() == "A" )
			{
				metaLineData.print(0);
			}
			if ( metaLineData.getType() == "P" )
			{
				if ( metaLineData.getDescriptor() == "run" )
				{
					// printf("test2\n");
					runtime = RTKey["processor"];
					metaLineData.print(runtime);
				}
			}	
			if ( metaLineData.getType() == "I" )
			{
				// printf("test1\n");
				// std::cout << metaLineData.getDescriptor() << std::endl;
				// int time;
				if ( metaLineData.getDescriptor() == "harddrive" )
				{
					// printf("test2\n");
					runtime = RTKey["harddrive"];
					metaLineData.print(runtime);
				}
				if ( metaLineData.getDescriptor() == "scanner" )
				{
					runtime = RTKey["scanner"];
					metaLineData.print(runtime);
				}
				if ( metaLineData.getDescriptor() == "keyboard" )
				{
					runtime = RTKey["keyboard"];
					metaLineData.print(runtime);
				}
			}
			if ( metaLineData.getType() == "O" )
			{
				// printf("test1\n");
				// std::cout << metaLineData.getDescriptor() << std::endl;
				if ( metaLineData.getDescriptor() == "harddrive" )
				{
					// printf("test2\n");
					runtime = RTKey["harddrive"];
					metaLineData.print(runtime);
				}
				if ( metaLineData.getDescriptor() == "monitor" )
				{
					runtime = RTKey["monitor"];
					metaLineData.print(runtime);
				}
				if ( metaLineData.getDescriptor() == "projector" )
				{
					runtime = RTKey["projector"];
					metaLineData.print(runtime);
				}
			}
			if ( metaLineData.getType() == "M" )
			{
				runtime = RTKey["memory"];
				metaLineData.print(runtime);
			}

		}
	}

};


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


#endif /* META_IO */