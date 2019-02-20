#include "test.h"
#include "MetaIO.h"
#include "ConfigIO.h"

bool doThis()
{
	// printf("Hello world\n");

	ConfigIO testConfigIO("../src/testConfig.txt", "log_file_name.txt");
	testConfigIO.readFile();

	std::string metaDataPath;
	if ( !testConfigIO.getMetaPath(metaDataPath) )
		return false;

	MetaIO testMetaIO("../src/" + metaDataPath);
	testMetaIO.readFile();


	return true;
}