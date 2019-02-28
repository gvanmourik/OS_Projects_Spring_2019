#ifndef CONFIG_DATA
#define CONFIG_DATA

// #define MSEC "msec"
// #define USEC "usec"

#include <map>
#include <string>
#include <algorithm>
#include <unordered_set>

#define BOTH 	"LogtoBoth"
#define MONITOR "LogtoMonitor"
#define FILE 	"LogtoFile"

typedef std::map<std::string, int> runtime_key_t;
std::unordered_set<std::string> LogType = {BOTH, 
										   FILE,
										   MONITOR}; 

// enum descriptor_t {HD,
// 					    KEYS,
// 					    SCANNER,
// 					    MONITOR,
// 					    PROJECTOR};
// enum run_time_t {MSEC="msec", USEC="usec"}; //only use milliseconds for now


class ConfigData 
{
private:
	int runTime;
	std::string Descriptor;


public:

	int getRuntime() { return runTime; }
	std::string getDescriptor() { return Descriptor; }

	// extracts the data while checking the format
	bool extractData(const std::string &s)
	{
		// if decimal is found, format is incorrect
		auto period = s.find('.');
  		if ( period != std::string::npos )
  			return false;

		// check if descriptor is followed with the correct format
		std::string descriptor = s.substr( 0, s.find(" cycle time {") );
		if ( descriptor == s )
		{
			descriptor = s.substr( 0, s.find(" display time {") );
			if ( descriptor == s )
				return false;
		}
		Descriptor = descriptor;

		// check if the units are an acceptable type
		auto temp = s.substr( s.find("{"), s.find("}:") ); //not sure why temp step is needed
		auto units = temp.substr( 1, temp.find("}")-1 );
		if ( units != "msec" )
			return false;
		// printf("%s\n", s.c_str());
		// printf("%s\n", Descriptor.c_str());

		// collect the runtime of the device
		auto time = s.substr( s.find(":")+1 );
		time.erase( std::remove_if(time.begin(), time.end(), isspace), time.end() );
		if ( time.empty() )
			return false;
		//verify that the string only consists of digits
		if ( !std::all_of(time.begin(), time.end(), ::isdigit) )
			return false;
		runTime = std::atoi( time.c_str() );

		return true;
	}

	void print()
	{
		std::cout << "\t" << Descriptor << " = " << runTime << " ms/cycle" << std::endl;
	}


};



#endif /* CONFIG_DATA */