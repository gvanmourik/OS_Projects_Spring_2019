#ifndef META_DATA
#define META_DATA

/// meta_data_t:
// 		S = START, A = APPLICATION,
// 		P = PROCESS, I = INPUT,
// 		O = OUPUT, M = MEMORY
enum meta_data_t {S, A, P, I, O, M, UN};
enum meta_descriptor_t {BEGIN,
					 	FINISH,
					 	HD,
					 	KEYS,
					 	SCANNER,
					 	MONITOR,
					 	RUN,
					 	ALLOCATE,
					 	PROJECTOR,
					 	BLOCK,
					 	UNASSIGNED};


class MetaData
{
private:
	meta_data_t Type;
	meta_descriptor_t Descriptor;
	unsigned int Count;

public:
	MetaData() : Type(UN), Descriptor(UNASSIGNED), Count(0) {}
	MetaData(meta_data_t _Type, meta_descriptor_t _Descriptor, unsigned int _Count) : 
		Type(_Type), Descriptor(_Descriptor), Count(_Count) {}
	~MetaData(){}

	void setType(meta_data_t newType) { Type = newType; }
	void setDescriptor(meta_descriptor_t newDescriptor) { Descriptor = newDescriptor; }
	void setCount(unsigned int newCount) { Count = newCount; }
	meta_data_t getType() { return Type; }
	meta_descriptor_t getDescriptor() { return Descriptor; }
	unsigned int getCount() { return Count; }

};



#endif /* META_DATA */