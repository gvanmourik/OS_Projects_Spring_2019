# Project 1

### Prerequisites
[CMake](https://cmake.org/download/) (Version >= 3.0)

### Building
```
mkdir build
cd build
cmake ../
make
```

### Running
Navigate to the build directory.

To run the test config file:
```
./bin/sim01 ../src/testConfig.conf
```

To run other config files:
```
./bin/sim01 <file_path>
```

### Structure
The structure of the included source files is the following:

main.cpp -- Handler.h -- -- ConfigIO.h -- ConfigData.h
						|
						|
						MetaIO.h -- MetaData.h

### Notes
* The log file is saved to build/
* The test config file is located in src/