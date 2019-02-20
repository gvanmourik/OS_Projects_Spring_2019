#ifndef META_IO
#define META_IO

#include <vector>
#include <fstream>
#include <iostream>
#include <type_traits>

#include "MetaData.h"

#define UNASSIGNED "NAH"

class MetaIO 
{
private:
	std::string FilePath;
	std::vector<MetaData> FileData;


public:
	/// Constructors
	MetaIO(std::string _filePath) : FilePath(_filePath){}
	~MetaIO(){}


	// Access functions
	std::string getPath() { return FilePath; }
	

	// Other functions
	bool readFile()
	{
		std::string currentStr;
		std::ifstream metaFile(FilePath);
		if ( metaFile.is_open() )
		{
			printf("meta file is open...\n");

			std::getline(metaFile, currentStr);
			if ( currentStr != "Start Program Meta-Data Code:" )
			{
				//throw 'file not formatted correctly' error
				return false;
			}


			MetaData lineData;
			std::string dataStr;
			char closingChar = '0';
			// std::getline(metaFile, currentStr, ' ');
			// auto closingChar = *(currentStr.end()-1);
			// auto dataStr = currentStr.substr(0, currentStr.find(closingChar));
			
			while ( closingChar != '.' )
			{
				std::getline(metaFile, currentStr, ' ');
				closingChar = *(currentStr.end()-1);
				dataStr = currentStr.substr(0, currentStr.find(closingChar));

				std::cout << dataStr << std::endl;
				std::cout << closingChar << std::endl;

				if ( !lineData.extractData(dataStr) )
				{
					printf("ERROR!!!\n");
					//throw 'file not formatted correctly' error
					return false;
				}
				FileData.push_back(lineData);
			}


		}
		else
		{
			printf("meta file is NOT open...\n");

			//throw 'file not open' error
			return false;
		}

		printf("done.......\n");

		return true;
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