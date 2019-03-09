# Project 2

### Prerequisites:
[CMake](https://cmake.org/download/) (Version >= 3.0)
<br>[Boost](https://www.boost.org/doc/libs/1_69_0/doc/html/quickbook/install.html) (Mac OS X, Windows, Debian, and Ubuntu)

### Building:

To generate the makefiles and compile:
```
./run_build
```

To remove the build files:
```
./remove_build
```

### Running:
Navigate to the build directory.

To run the test config file:
```
./bin/sim02 ../src/testConfig.conf
```

To run other config files:
```
./bin/sim02 <file_path>
```

### Key Lines (PCB and Threads)

#### Process Control Block (PCB)
**Defined in src/PCB.h**

**Created at:**
MetaIO.h [lines 125 and 167]
```
PCBlocks[processCount] = new PCB(processCount);
```
**State Modified at:**
MetaIO.h [lines 171, 397, 409, 415, 426, 427, 441, 442, 456, 457, 461, 467, 480, 481, 495, 496, 510, 511, 515, 520, 530, 537, 540]
```
PCBlocks[processCount]->setState(<state>);
```

#### Threads
**Used in src/MetaIO.h**

**Variables [lines 42-46]**
```
pthread_t thread;
pthread_mutex_t mutex;
sem_t* semaphore;
unsigned int semValue;
const char* semName = "sem";
```
**Initialized [lines 64-66]**
```
pthread_mutex_init(&mutex, nullptr);
semaphore = sem_open(semName, O_CREAT | O_EXCL, 0644, semValue);
sem_unlink(semName);
```
**Lock/Unlock [lines 102 and 178]**
```
pthread_mutex_lock(&mutex);
pthread_mutex_unlock(&mutex);
```
**Semaphore and pthread used in I/O operations [lines 423-513]**



### Structure:
The structure of the included source files is the following:

main.cpp -- Handler.h -- -- ConfigIO.h -- ConfigData.h 	<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|								<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|								<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MetaIO.h -- MetaData.h          <br />

### Notes:
* The log file is saved to build/
* The test config file is located in src/