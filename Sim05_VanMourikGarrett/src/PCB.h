#ifndef PCB_H
#define PCB_H

#include <unordered_map>

enum ProcessState {START, READY, RUNNING, WAIT, EXIT};

class PCB 
{
private:
	int processID;
	int processIOCount;
	ProcessState processState;

public:
	PCB(int _processID, int _processIOCount = 0, ProcessState state=START) : 
		processID(_processID), processIOCount(_processIOCount), processState(state) {}
	~PCB() {}

	//Access functions
	int getID() const { return processID; }
	int getIOCount() const { return processIOCount; }
	ProcessState getState() const { return processState; }
	void setIOCount(int IOCount) { processIOCount = IOCount; } 
	void setState(ProcessState state) { processState = state; }

};

typedef std::unordered_map<int, PCB*> process_map_t;


#endif /* PCB_H */