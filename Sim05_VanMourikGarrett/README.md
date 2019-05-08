
<!-- *********************************** -->
# Project 5
<!-- *********************************** -->



## Prerequisites:
[CMake](https://cmake.org/download/) (Version >= 3.5.1)
<br>[Boost](https://www.boost.org/doc/libs/1_69_0/doc/html/quickbook/install.html) (Mac OS X, Windows, Debian, and Ubuntu)



## Building:
To generate the makefiles and compile:
```
./run_build
```

To remove the build files:
```
./remove_build
```



## Running:
Navigate to the build directory.

To run the test config file:
```
./bin/sim05 ../TestFiles/testConfig.conf
```

To run other config files (and associated meta data files):

* First, place files in the **TestFiles** directory.

* Second, call the following command, replacing $FILENAME with the file name. 

```
./bin/sim05 ../TestFiles/$FILENAME.conf
```



## Running on a headless session of the ECC server:
Login to server with:
```
ssh username@ubuntu.cse.unr.edu
```

Navigate to a directory **($ROOT)** to download the source code.
Download the code with:
```
git clone https://github.com/gvanmourik/OS_Projects_Spring_2019.git
```

Navigate to the Project 4 directory:
```
cd OS_Projects_Spring_2019/Sim05_VanMourikGarrett/
```

Build the project with:
```
./run_build
```

Navigate to the build directory:
```
cd build/
```

Add config and meta data files to TestFiles/ **(from local computer)**:
```
scp $FILENAME.conf username@ubuntu.cse.unr.edu:$ROOT/OS_Projects_Spring_2019/Sim05_VanMourikGarrett/TestFiles
scp $FILENAME.mdf username@ubuntu.cse.unr.edu:$ROOT/OS_Projects_Spring_2019/Sim05_VanMourikGarrett/TestFiles
```

Run the simulation **(on server)**:
```
./bin/sim05 ../TestFiles/$FILENAME.conf
```


## Project Specifications

### Resource Management System
**Created and initialized in:** src/ConfigIO.h

**Utilized in:** src/MetaIO.h

**Description:** Implemented with a map, the key is a string with the resource description, and the value is a vector of booleans that track whether or not a particular resource is in use. Because this project was designed with concurrency in mind yet does not currently implement forking, the output will only show that *one resource* is being used at a time. Once forking is implemented, the output will display multiple resource usage.


### Scheduling Algorithms
**Implemented in:**
src/MetaIO.h
* First-in First-out (FIFO): lines 286-292
* Shortest Job First (SJF): lines 293-309
* Priority Scheduling (PS): lines 310-331


### Memory Allocation
**Defined and called in:**
src/MetaIO.h


### Process Control Block (PCB)
**Defined in:** 
src/PCB.h

**Created in:**
MetaIO.h
```
PCBlocks[processCount] = new PCB(processCount);
```
**State Modified in:**
MetaIO.h
```
PCBlocks[processCount]->setState(<state>);
```

### Threads
**Used in:** 
src/MetaIO.h

**Variables**
```
pthread_t thread;
pthread_mutex_t mutex;
sem_t* semaphore;
unsigned int semValue;
const char* semName = "sem";
```
**Initialized**
```
pthread_mutex_init(&mutex, nullptr);
semaphore = sem_open(semName, O_CREAT | O_EXCL, 0644, semValue);
sem_unlink(semName);
```
**Lock/Unlock**
**Called in:** src/MetaIO.h **Lines[448,458,487,497,...]**
```
pthread_mutex_lock(&mutex);
pthread_mutex_unlock(&mutex);
```
**Semaphores**

Calls to sem_wait() were removed, as some of the ECC server nodes are missing sem_wait.c, which causes a seg fault.

### Structure:
The structure of the included source files is the following:

main.cpp -- Handler.h -- -- ConfigIO.h -- ConfigData.h 	<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|								<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|								<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MetaIO.h -- MetaData.h          <br />


### Notes:
* The log file is saved to build/