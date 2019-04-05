#ifndef META_IO
#define META_IO

#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <type_traits>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "PCB.h"
#include "Timer.h"
#include "ConfigData.h"
#include "MetaData.h"

typedef void * (*thread_func_ptr)(void *);


class MetaIO 
{
private:
	bool startFlag = false;
	bool systemStartFlag = false;
	bool appStartFlag = false;
	int systemMemory;
	int processCount = 0;
	Timer timer;
	std::string FilePath;
	std::vector<MetaData> FileData;
	process_map_t PCBlocks;
	runtime_key_t rtKey;

	//multithreading
	void* time_ptr;
	pthread_t thread;
	pthread_mutex_t mutex;
	sem_t* semaphore;
	unsigned int semValue;
	const char* semName = "sem";


public:
	/// Constructors
	MetaIO(std::string _filePath, int _systemMemory, runtime_key_t _rtKey) : 
		FilePath(_filePath), rtKey(_rtKey), systemMemory(_systemMemory){}
	~MetaIO(){}


	// Access functions
	std::string getPath() { return FilePath; }
	

	// Other functions
	bool readFile(std::vector<std::string> &errlog)
	{
		//init pthread and semaphore
		pthread_mutex_init(&mutex, nullptr);
		semaphore = sem_open(semName, O_CREAT | O_EXCL, 0644, semValue);
		sem_unlink(semName);

		std::string currentStr;
		std::ifstream metaFile(FilePath);
		if (metaFile.peek() == std::ifstream::traits_type::eof())
		{
			errlog.push_back(" ERROR: Meta-Data file is empty or does not exist!\n");
			return false;
		}

		if ( metaFile.is_open() )
		{
			// std::cout << "file is open..." << std::endl;
			// std::cout << "Meta-Data Metrics:" << std::endl;
			if ( !readline(metaFile, currentStr) )
			{
				errlog.push_back(" ERROR: Meta-data file not formatted correctly! {start/end section}\n");
				return false;
			}

			MetaData lineData;
			std::string dataStr;
			
			/// Collect meta-data
			//start timer
			timer.start();
			if ( !readline(metaFile, currentStr) )
			{
				errlog.push_back(" ERROR: Meta-data file not formatted correctly! {start/end section}\n");
				return false;
			}
			bool lastMetaDataLine;
			if ( *(currentStr.end()-1) == '.' )
				lastMetaDataLine = true;
			
			//lock thread
			pthread_mutex_lock(&mutex);
			
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
					if ( !checkFlags(lineData) )
					{
						errlog.push_back(" ERROR: Application begin/finish not configured correctly!\n");
						return false;
					}
					if ( lineData.getType() == "A" && lineData.getDescriptor() == "begin" )
					{
						processCount++;
						PCBlocks[processCount] = new PCB(processCount);
					}
					if ( lineData.getType() == "A" && lineData.getDescriptor() == "finish" )
					{
						PCBlocks[processCount]->setState(EXIT);
					}
					printLine(processCount, lineData, rtKey);
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
				if ( !checkFlags(lineData) )
				{
					errlog.push_back(" ERROR: Application begin/finish not configured correctly!\n");
					return false;
				}
				if ( lineData.getType() == "A" && lineData.getDescriptor() == "begin" )
				{
					processCount++;
					PCBlocks[processCount] = new PCB(processCount);
				}
				if ( lineData.getType() == "A" && lineData.getDescriptor() == "finish" )
				{
					PCBlocks[processCount]->setState(EXIT);
				}
				printLine(processCount, lineData, rtKey);
				FileData.push_back(lineData);
		    }

		    //unlock thread
			pthread_mutex_unlock(&mutex);

		    std::cout << std::endl;
		    if ( !finalFlagCheck(errlog) )
		    	return false;

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

	bool checkFlags(MetaData &lineData)
	{
		if ( lineData.getType() == "S" )
		{
			if (!systemStartFlag && lineData.getDescriptor() == "begin")
			{
				systemStartFlag = true;
				return true;
			}
			if (systemStartFlag && lineData.getDescriptor() == "finish")
			{
				systemStartFlag = false;
				return true;
			}
			return false;
		}
		if ( lineData.getType() == "A" )
		{
			if (!appStartFlag && lineData.getDescriptor() == "begin")
			{
				appStartFlag = true;
				return true;
			}
			if (appStartFlag && lineData.getDescriptor() == "finish")
			{
				appStartFlag = false;
				return true;
			}
			return false;
		}
		//make sure that both the system and application have been started before
		// processing any other input
		else
		{
			if (!systemStartFlag || !appStartFlag)
				return false;
		}
		return true;
	}

	bool finalFlagCheck(std::vector<std::string> &errlog)
	{
		if ( appStartFlag )
	    {
	    	errlog.push_back(" ERROR: Application begin/finish not configured correctly!\n");
			return false;
	    }
	    if ( systemStartFlag )
	    {
	    	errlog.push_back(" ERROR: System begin/finish not configured correctly!\n");
			return false;
	    }
	    return true;
	}

	void print(runtime_key_t RTKey)
	{
		std::cout << "Meta-Data Metrics:" << std::endl;
		for (auto MD : FileData)
		{
			int runtime;
			if ( MD.getType() == "S" )
			{
				MD.print(0);
			}
			if ( MD.getType() == "A" )
			{
				MD.print(0);
			}
			if ( MD.getType() == "P" )
			{
				if ( MD.getDescriptor() == "run" )
				{
					// printf("test2\n");
					runtime = RTKey["processor"];
					MD.print(runtime);
				}
			}	
			if ( MD.getType() == "I" )
			{
				// printf("test1\n");
				// std::cout << MD.getDescriptor() << std::endl;
				// int time;
				if ( MD.getDescriptor() == "harddrive" )
				{
					// printf("test2\n");
					runtime = RTKey["harddrive"];
					MD.print(runtime);
				}
				if ( MD.getDescriptor() == "scanner" )
				{
					runtime = RTKey["scanner"];
					MD.print(runtime);
				}
				if ( MD.getDescriptor() == "keyboard" )
				{
					runtime = RTKey["keyboard"];
					MD.print(runtime);
				}
			}
			if ( MD.getType() == "O" )
			{
				// printf("test1\n");
				// std::cout << MD.getDescriptor() << std::endl;
				if ( MD.getDescriptor() == "harddrive" )
				{
					// printf("test2\n");
					runtime = RTKey["harddrive"];
					MD.print(runtime);
				}
				if ( MD.getDescriptor() == "monitor" )
				{
					runtime = RTKey["monitor"];
					MD.print(runtime);
				}
				if ( MD.getDescriptor() == "projector" )
				{
					runtime = RTKey["projector"];
					MD.print(runtime);
				}
			}
			if ( MD.getType() == "M" )
			{
				runtime = RTKey["memory"];
				MD.print(runtime);
			}

		}
	}

	bool printLine(int processID, MetaData &MD, runtime_key_t RTKey)
	{
		pthread_mutex_init(&mutex, nullptr);
		double runtime, cycles, waitTime;

		//system
		if ( MD.getType() == "S" )
		{	
			if ( MD.getDescriptor() == "begin" )
			{
				std::cout << getTimeString() << " -- Simulator program starting" << std::endl;
			}
			if ( MD.getDescriptor() == "finish" )
			{
				std::cout << getTimeString() << " -- Simulator program ending" << std::endl;
			}
			return true;
		}

		// application
		if ( MD.getType() == "A" )
		{
			if ( MD.getDescriptor() == "begin" )
			{
				std::cout << getTimeString() << " -- OS: preparing process " << processID << std::endl;
				std::cout << getTimeString() << " -- OS: starting process " << processID << std::endl;
			}
			if ( MD.getDescriptor() == "finish" )
			{
				std::cout << getTimeString() << " -- OS: removing process " << processID << std::endl;
			}
			return true;
		}

		std::string processorName = " -- Process " + std::to_string(processID) + ": ";
		if ( MD.getType() == "P" )
		{
			//set process state
			PCBlocks[processCount]->setState(READY);
			
			if ( MD.getDescriptor() == "run" )
			{
				runtime = RTKey["processor"];
				cycles = (double)MD.getCycles();

				std::cout << getTimeString() << processorName << "start processing action" << std::endl;
				wait(runtime * cycles);
				PCBlocks[processCount]->setState(RUNNING); //part of the system
				std::cout << getTimeString() << processorName << "end processing action" << std::endl;
			}
			PCBlocks[processCount]->setState(READY);
		}
		//input operation	
		if ( MD.getType() == "I" )
		{
			//set process state
			PCBlocks[processCount]->setState(READY);

			if ( MD.getDescriptor() == "harddrive" )
			{
				runtime = RTKey["harddrive"];
				cycles = (double)MD.getCycles();
				waitTime = runtime * cycles;

				sem_wait(semaphore);
				std::cout << getTimeString() << processorName << "start hard drive input" << std::endl;
				pthread_mutex_lock(&mutex);
				pthread_create(&thread, nullptr, MetaIO::wait_thread, (void*)&waitTime);
				PCBlocks[processCount]->setState(RUNNING);
				PCBlocks[processCount]->setState(WAIT);
				pthread_mutex_unlock(&mutex);
				pthread_join(thread, nullptr);
				std::cout << getTimeString() << processorName << "end hard drive input" << std::endl;
				sem_post(semaphore);
			
			}
			if ( MD.getDescriptor() == "scanner" )
			{
				runtime = RTKey["scanner"];
				cycles = (double)MD.getCycles();
				waitTime = runtime * cycles;

				sem_wait(semaphore);
				std::cout << getTimeString() << processorName << "start scanner input" << std::endl;
				pthread_mutex_lock(&mutex);
				pthread_create(&thread, nullptr, MetaIO::wait_thread, (void*)&waitTime);
				PCBlocks[processCount]->setState(RUNNING);
				PCBlocks[processCount]->setState(WAIT);
				pthread_mutex_unlock(&mutex);
				pthread_join(thread, nullptr);
				std::cout << getTimeString() << processorName << "end scanner input" << std::endl;
				sem_post(semaphore);
			
			}
			if ( MD.getDescriptor() == "keyboard" )
			{
				runtime = RTKey["keyboard"];
				cycles = (double)MD.getCycles();
				waitTime = runtime * cycles;

				sem_wait(semaphore);
				std::cout << getTimeString() << processorName << "start keyboard input" << std::endl;
				pthread_mutex_lock(&mutex);
				pthread_create(&thread, nullptr, MetaIO::wait_thread, (void*)&waitTime);
				PCBlocks[processCount]->setState(RUNNING);
				PCBlocks[processCount]->setState(WAIT);
				pthread_mutex_unlock(&mutex);
				pthread_join(thread, nullptr);
				std::cout << getTimeString() << processorName << "end keyboard input" << std::endl;
				sem_post(semaphore);
			
			}
			PCBlocks[processCount]->setState(READY);
		}
		//output operation
		if ( MD.getType() == "O" )
		{
			//set process state
			PCBlocks[processCount]->setState(READY);

			if ( MD.getDescriptor() == "harddrive" )
			{
				// printf("test2\n");
				runtime = RTKey["harddrive"];
				cycles = (double)MD.getCycles();
				waitTime = runtime * cycles;

				sem_wait(semaphore);
				std::cout << getTimeString() << processorName << "start hard drive output" << std::endl;
				pthread_mutex_lock(&mutex);
				pthread_create(&thread, nullptr, MetaIO::wait_thread, (void*)&waitTime);
				PCBlocks[processCount]->setState(RUNNING);
				PCBlocks[processCount]->setState(WAIT);
				pthread_mutex_unlock(&mutex);
				pthread_join(thread, nullptr);
				std::cout << getTimeString() << processorName << "end hard drive output" << std::endl;
				sem_post(semaphore);
			
			}
			if ( MD.getDescriptor() == "monitor" )
			{
				runtime = RTKey["monitor"];
				cycles = (double)MD.getCycles();
				waitTime = runtime * cycles;

				sem_wait(semaphore);
				std::cout << getTimeString() << processorName << "start monitor output" << std::endl;
				pthread_mutex_lock(&mutex);
				pthread_create(&thread, nullptr, MetaIO::wait_thread, (void*)&waitTime);
				PCBlocks[processCount]->setState(RUNNING);
				PCBlocks[processCount]->setState(WAIT);
				pthread_mutex_unlock(&mutex);
				pthread_join(thread, nullptr);
				std::cout << getTimeString() << processorName << "end monitor output" << std::endl;
				sem_post(semaphore);
			}
			if ( MD.getDescriptor() == "projector" )
			{
				runtime = RTKey["projector"];
				cycles = (double)MD.getCycles();
				waitTime = runtime * cycles;

				sem_wait(semaphore);
				std::cout << getTimeString() << processorName << "start projector output" << std::endl;
				pthread_mutex_lock(&mutex);
				pthread_create(&thread, nullptr, MetaIO::wait_thread, (void*)&waitTime);
				PCBlocks[processCount]->setState(RUNNING);
				PCBlocks[processCount]->setState(WAIT);
				pthread_mutex_unlock(&mutex);
				pthread_join(thread, nullptr);
				std::cout << getTimeString() << processorName << "end projector output" << std::endl;
				sem_post(semaphore);
			}
			PCBlocks[processCount]->setState(READY);
		}
		if ( MD.getType() == "M" )
		{
			//set process state
			PCBlocks[processCount]->setState(READY);

			runtime = RTKey["memory"];
			cycles = (double)MD.getCycles();
			// MD.print(runtime);

			if ( MD.getDescriptor() == "allocate" )
			{
				std::cout << getTimeString() << processorName << "allocating memory" << std::endl;
				wait(runtime * cycles);
				PCBlocks[processCount]->setState(RUNNING);
				std::cout << getTimeString() << processorName << "memory allocated at 0x" << allocateMemory() << std::endl;
			}
			if ( MD.getDescriptor() == "block" )
			{
				std::cout << getTimeString() << processorName << "start memory blocking" << std::endl;
				wait(runtime * cycles);
				PCBlocks[processCount]->setState(RUNNING);
				std::cout << getTimeString() << processorName << "end memory blocking" << std::endl;
			}
			PCBlocks[processCount]->setState(READY);
		}
		return true;
	}

	std::string allocateMemory()
	{
		// auto max = std::round(systemMemory);
		// std::cout << "max = " << max << std::endl;

		timeval t;
	 	gettimeofday(&t, nullptr);
	  	boost::mt19937 seed( (int)t.tv_usec );
		boost::random::uniform_int_distribution<> dist(0, std::round(systemMemory));
		boost::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<> > random(seed, dist);
		
		std::stringstream ss;
		auto temp = random();
		ss << std::hex << temp;

		int numBits = 8;
		auto memString = ss.str();
		if ( memString.length() < numBits )
		{
			auto addBits = numBits - memString.length();
			for (int i=0; i < addBits; ++i)
			{
				memString = "0" + memString;
			}
		}


		return memString;
	}

	void wait(double delayTime)
	{
		Timer timer;
		timer.start();

		while ( timer.mSec() < delayTime ){}
	}

	static void* wait_thread(void* delayTime)
	{
		Timer timer;
		timer.start();

		//wait
		while ( timer.mSec() < *(double*)delayTime ){}
		pthread_exit(nullptr);
	}

	std::string getTimeString()
	{
		double long time = timer.sec();
		return std::to_string(time) + " sec";
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