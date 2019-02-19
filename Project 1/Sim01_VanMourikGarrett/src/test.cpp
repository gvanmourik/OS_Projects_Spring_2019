#include "test.h"
#include "FileIO.h"

void doThis()
{
	// printf("Hello world\n");

	FileIO<ConfigData> testConfigIO("../src/testConfig.txt", "log_file_name.txt");
	testConfigIO.readFile();
}