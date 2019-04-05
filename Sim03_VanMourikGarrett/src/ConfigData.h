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

typedef std::map<std::string, double> runtime_key_t;
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
	double runTimeValue;
	std::string units;
	std::string Descriptor;


public:

	int getValue() { return runTimeValue; }
	std::string getDescriptor() { return Descriptor; }

	// extracts the data while checking the format
	bool extractData(const std::string &s)
	{
		// if decimal is found, format is incorrect
		auto period = s.find('.');
  		if ( period != std::string::npos )
  			return false;
		
		//debug
		// std::cout << "Before descriptor..." << std::endl;
	
  		if ( !getDescriptor(s, Descriptor) )
  			return false;
		// Descriptor = descriptor;

  		//debug
		// std::cout << "After descriptor..." << std::endl;

		// check if the units are an acceptable type
		auto temp = s.substr( s.find("{"), s.find("}:") ); //not sure why temp step is needed
		units = temp.substr( 1, temp.find("}")-1 );
		if ( units != "msec" && 
			 units != "kbytes" && 
			 units != "Mbytes" &&
			 units != "Gbytes" )  
		{ return false; }

		//debug
		// std::cout << "After units..." << std::endl;

		// collect the runTimeValue of the device
		auto value = s.substr( s.find(":")+1 );
		value.erase( std::remove_if(value.begin(), value.end(), isspace), value.end() );
		if ( value.empty() )
			return false;
		//verify that the string only consists of digits
		if ( !std::all_of(value.begin(), value.end(), ::isdigit) )
			return false;
		
		runTimeValue = std::atof( value.c_str() );
		if ( units == "Mbytes" || units == "Gbytes" )
		{
			//convert to kbytes
			convert();
		}

		return true;
	}

	bool getDescriptor(const std::string &s, std::string &d)
	{
		//debug
		// std::cout << "Before format check..." << std::endl;

		if ( !(s.find(" cycle time {") != std::string::npos ||
			   s.find(" display time {") != std::string::npos ||
			   s.find("System memory") != std::string::npos) )
		{ return false; }

		//debug
		// std::cout << "After format check..." << std::endl;

		bool descriptorSet = false;
		std::string descriptor;

		descriptor = s.substr( 0, s.find(" cycle time {") );
		if ( descriptor != s )
		{
			d = descriptor;
			descriptorSet = true;
		}
		descriptor = s.substr( 0, s.find(" display time {") );
		if ( descriptor != s )
		{
			d = descriptor;
			descriptorSet = true;
		}
		if ( !descriptorSet )
		{
			d = "System memory";
		}

		//debug
		// std::cout << "After descriptor assignment..." << std::endl;

		return true;
	}

	void convert()
	{
		if ( units == "Mbytes" )
		{
			runTimeValue *= 1e3;
		}
		if ( units == "Gbytes" )
		{
			runTimeValue *= 1e6;
		}
		units = "kbytes";
	}

	void print()
	{
		if ( units == "msec" )
			std::cout << "\t" << Descriptor << " = " << runTimeValue << " ms/cycle" << std::endl;
		if ( units == "kbytes" )
		{
			if ( runTimeValue > 1e6 )
			{
				std::cout << "\t" << Descriptor << " = " << std::scientific;
				std::cout << (double)runTimeValue << " kbytes" << std::endl;
			}
			else
			{
				std::cout << "\t" << Descriptor << " = " << runTimeValue << " kbytes" << std::endl;
			}
		}
	}


};



#endif /* CONFIG_DATA */