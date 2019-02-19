#ifndef CONFIG_DATA
#define CONFIG_DATA


// enum meta_descriptor_t {HD,
// 					    KEYS,
// 					    SCANNER,
// 					    MONITOR,
// 					    PROJECTOR};
enum run_time_t {MSEC, USEC}; //only use milliseconds for now


class ConfigData 
{
private:
	double runTime;
	run_time_t runTimeUnits;
	std::string DeviceType;


public:
	

	
};



#endif /* CONFIG_DATA */