#ifndef CONFIG_DATA
#define CONFIG_DATA

// #define MSEC "msec"
// #define USEC "usec"

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

#define BOTH 	"LogtoBoth"
#define MONITOR "LogtoMonitor"
#define FILE 	"LogtoFile"

#define AVAILABLE true
#define BUSY true
#define NO_LIMIT -1

typedef std::map<std::string, double> runtime_key_t;
typedef std::map<std::string, std::vector<bool>> resource_t;
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
	double value;
	std::string units;
	std::string Descriptor;
	bool isResourceQuantity = false;


public:

	int getValue() { return value; }
	std::string getDescriptor() { return Descriptor; }
	bool isResource() { return isResourceQuantity; }

	// extracts the data while checking the format
	bool extractData(const std::string &s)
	{
		// if decimal is found, format is incorrect
		auto period = s.find('.');
  		if ( period != std::string::npos )
  			return false;
		
		//debug
		// std::cout << "Before descriptor..." << std::endl;
	
  		if ( !checkDescriptor(s, Descriptor) )
  			return false;

  		//debug
		// std::cout << "After descriptor..." << std::endl;


		if ( isResourceQuantity )
		{
			//collect quantity
			auto valueStr = s.substr( s.find(":")+1 );
			valueStr.erase( std::remove_if(valueStr.begin(), valueStr.end(), isspace), valueStr.end() );
			
			if ( valueStr.empty() )
				return false;

			//verify that the string only consists of digits
			if ( !std::all_of(valueStr.begin(), valueStr.end(), ::isdigit) )
				return false;
			
			//set value
			value = std::atof( valueStr.c_str() );

			//set resource descriptor
			setResourceDescriptor();

			return true;
		}


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

		// collect the value of the device
		auto valueStr = s.substr( s.find(":")+1 );
		valueStr.erase( std::remove_if(valueStr.begin(), valueStr.end(), isspace), valueStr.end() );
		if ( valueStr.empty() )
			return false;
		//verify that the string only consists of digits
		if ( !std::all_of(valueStr.begin(), valueStr.end(), ::isdigit) )
			return false;
		
		value = std::atof( valueStr.c_str() );
		if ( units == "Mbytes" || units == "Gbytes" )
		{
			convertToKB();
		}

		return true;
	}

	bool checkDescriptor(const std::string &s, std::string &d)
	{
		//debug
		// std::cout << "Before format check..." << std::endl;
		
		bool cycle_time = s.find(" cycle time {") != std::string::npos;
		bool display_time = s.find(" display time {") != std::string::npos;
		bool resource_quantity = s.find(" quantity:") != std::string::npos;
		bool sys_memory = s.find("System memory") != std::string::npos;
		bool sys_memory_size = s.find("Memory block size") != std::string::npos;

		if ( !(cycle_time 		||
			   display_time 	||
			   resource_quantity||
			   sys_memory 		||
			   sys_memory_size 
			   ))
		{ return false; }

		//debug
		// std::cout << "After format check..." << std::endl;

		bool descriptorSet = false;
		bool isMemorySpec = true;
		std::string descriptor;

		if ( cycle_time )
		{
			setDescriptor(d, s, descriptorSet, !isMemorySpec, " cycle time {");
		}
		if ( display_time )
		{
			setDescriptor(d, s, descriptorSet, !isMemorySpec, " display time {");
		}
		if ( resource_quantity )
		{
			isResourceQuantity = true;
			setDescriptor(d, s, descriptorSet, !isMemorySpec, " quantity:");
		}
		if ( sys_memory )
		{
			setDescriptor(d, s, descriptorSet, isMemorySpec, "System memory");
		}
		if ( sys_memory_size )
		{
			setDescriptor(d, s, descriptorSet, isMemorySpec, "Memory block size");
		}

		//debug
		// std::cout << "After descriptor assignment..." << std::endl;

		return descriptorSet;
	}

	void convertToKB()
	{
		if ( units == "Mbytes" )
		{
			value *= 1e3;
		}
		if ( units == "Gbytes" )
		{
			value *= 1e6;
		}
		units = "kbytes";
	}

	void setDescriptor(std::string &d, const std::string &s, bool &descriptorSet, bool isMemorySpec, std::string search)
	{
		if ( !isMemorySpec )
		{
			std::string descriptor = s.substr( 0, s.find(search) );
			if ( descriptor != s )
			{
				d = descriptor;
				descriptorSet = true;
			}
		}
		else
		{
			d = search;
			descriptorSet = true;
		}
	}

	void setResourceDescriptor()
	{
		if ( Descriptor == "Hard drive" )
			Descriptor = "HDD";
		if ( Descriptor == "Projector" )
			Descriptor = "PROJ";
		if ( Descriptor == "Monitor" )
			Descriptor = "MON";
		if ( Descriptor == "Keyboard" )
			Descriptor = "KEY";
		if ( Descriptor == "Scanner" )
			Descriptor = "SCAN";
	}

	void print()
	{
		if ( units == "msec" )
			std::cout << "\t" << Descriptor << " = " << value << " ms/cycle" << std::endl;
		if ( units == "kbytes" )
		{
			if ( value > 1e6 )
			{
				std::cout << "\t" << Descriptor << " = " << std::scientific;
				std::cout << (double)value << " kbytes" << std::endl;
			}
			else
			{
				std::cout << "\t" << Descriptor << " = " << value << " kbytes" << std::endl;
			}
		}
	}


};



#endif /* CONFIG_DATA */