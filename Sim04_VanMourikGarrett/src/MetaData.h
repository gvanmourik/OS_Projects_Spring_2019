#ifndef META_DATA
#define META_DATA

#include <map>
#include <unordered_set>

typedef std::map<std::string, std::unordered_set<std::string>> Key_t;

class MetaData
{
private:
	std::string OpType;
	std::string Descriptor;
	int Cycles;

	Key_t DescriptorKey; //associated descriptor set with each process type

public:
	MetaData(){ initKeys(); }


	/// Access funcitons
	std::string getType() { return OpType; }
	std::string getDescriptor() { return Descriptor; }
	int getCycles() { return Cycles; }


	/// Other functions
	bool extractData(const std::string &s)
	{
		// find the operation type
		auto type = s.substr(0, s.find('{'));
		if ( type.length() > 1 )
			return false;
		if ( typeNotDefined(type) )
			return false;
		OpType = type;
		// std::cout << "OpType = " << OpType << std::endl;

		// find the operation descriptor
		auto d = s.substr(s.find("{")+1, s.find("}")-s.find("{")-1);
		// std::cout << "d = " << d << std::endl;
		if ( descriptorNotDefined(d) )
			return false;
		Descriptor = d;

		auto numStr = s.substr(s.find("}")+1);
		if ( numStr.empty() )
			return false;
		//verify that the string only consists of digits
		if ( !std::all_of(numStr.begin(), numStr.end(), ::isdigit) )
			return false;
		Cycles = std::atoi(numStr.c_str());

		//if type == P
		// 


		return true;
	}

	void initKeys()
	{
		DescriptorKey["S"] = {"begin", "finish"};
		DescriptorKey["A"] = {"begin", "finish"};
		DescriptorKey["P"] = {"run"};
		DescriptorKey["I"] = {"harddrive", 
							  "scanner",
							  "keyboard"};
	    DescriptorKey["O"] = {"harddrive", 
	    					  "monitor",
	    					  "projector"};
	    DescriptorKey["M"] = {"block", 
	    					  "allocate"};
	}

	bool typeNotDefined(std::string operation)
	{
		if ( DescriptorKey.find(operation) == DescriptorKey.end() )
			return true;
		return false;
	}

	bool descriptorNotDefined(std::string descriptor)
	{
		auto set = DescriptorKey[OpType];
		if ( set.find(descriptor) == set.end() )
			return true;
		return false;
	}

	void print(int time)
	{
		std::cout << "\t" << OpType << "{" << Descriptor << "}";
		std::cout << Cycles << " - " << Cycles*time << " ms" << std::endl;
	}

};


// void waitForResource(std::string resource)
	// {
	// 	//get resource flags
	// 	auto flags = Resources[resource];

	// 	//wait until resource flag is available
	// 	while ( !flagAvailable(flags) ) {}
	// }

	// bool flagAvailable(const std::vector<bool> &flags)
	// {
	// 	for ( auto isAvailable : flags )
	// 	{
	// 		if ( isAvailable )
	// 			return isAvailable;
	// 	}
	// 	return false;
	// }

	// int getResourceNumber(const std::vector<bool> &flags)
	// {
	// 	for (int flag=0; flag < flags.size(); ++flag)
	// 	{
	// 		if ( flags[flag] == AVAILABLE )
	// 			return flag;			
	// 	}
	// 	return -1;
	// }


#endif /* META_DATA */