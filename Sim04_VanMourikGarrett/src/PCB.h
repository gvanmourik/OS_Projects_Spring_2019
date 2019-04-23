#ifndef PCB_H
#define PCB_H

#include <unordered_map>

enum ProcessState {START, READY, RUNNING, WAIT, EXIT};

class PCB 
{
private:
	int processID;
	ProcessState processState;

public:
	PCB(int _processID, ProcessState state=START) : processID(_processID), processState(state) {}
	~PCB() {}

	//Access functions
	int getID() const { return processID; }
	ProcessState getState() const { return processState; }
	void setState(ProcessState state) { processState = state; }

};

typedef std::unordered_map<int, PCB*> process_map_t;


#endif /* PCB_H */