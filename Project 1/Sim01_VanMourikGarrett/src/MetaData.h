#ifndef META_DATA
#define META_DATA

#include <map>

typedef std::map<std::string, bool> Key_t;

class MetaData
{
private:
	std::string OpType;
	std::string Descriptor;
	int Cycles;

	Key_t OperationKey;
	Key_t DescriptorKey;
	std::vector<std::string> validTypes = {"S", "A", "P", "I", "O", "M"};
	std::vector<std::string> validDescriptors = {"begin", 
												 "finish", 
												 "hard drive", 
												 "keys", 
												 "scanner",
												 "monitor",
												 "run",
												 "allocate",
												 "projector",
												 "block"};

public:
	MetaData(){ initKeys(); }


	/// Access funcitons
	std::string getType() { return OpType; }
	std::string getDescriptor() { return Descriptor; }
	unsigned int getCycles() { return Cycles; }


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

		// find the operation descriptor
		auto d = s.substr(s.find("{")+1, s.find("}")-s.find("{")-1);
		if ( descriptorNotDefined(d) )
			return false;

		auto numStr = s.substr(s.find("}")+1);
		//verify that the string only consists of digits
		if ( !std::all_of(numStr.begin(), numStr.end(), ::isdigit) )
			return false;
		Cycles = std::atoi(numStr.c_str());
		std::cout << numStr << std::endl;

		return true;
	}

	void initKeys()
	{
		for (auto type : validTypes)
			OperationKey[type] = true;
		for (auto descriptor : validDescriptors)
			DescriptorKey[descriptor] = true;
	}

	bool typeNotDefined(std::string operation)
	{
		if ( OperationKey.find(operation) == OperationKey.end() )
			return true;
		return false;
	}

	bool descriptorNotDefined(std::string descriptor)
	{
		if ( DescriptorKey.find(descriptor) == DescriptorKey.end() )
			return true;
		return false;
	}

};



#endif /* META_DATA */