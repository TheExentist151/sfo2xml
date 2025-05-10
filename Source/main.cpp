#include "Helpers.h"
#include "SFOFile.h"					

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("sfo2xml - SFO to XML converter (1.0)\nUsage: \n\nsfo2xml <input SFO or XML path> [output path]");
		return 0;
	}

	SFOFile sfoFile;
	char* pInputPath = argv[1];
	char* pOutputPath = NULL;

	if (argc >= 3)
	{
		pOutputPath = argv[2];
	}
	else
	{
		char* extension = help::GetExtension(pInputPath);
		if (strcmp(extension, "xml") == 0)
			pOutputPath = help::ChangeExtension(pInputPath, "SFO");
		else
			pOutputPath = help::ChangeExtension(pInputPath, "xml");
	}

	if (!help::FileExists(pInputPath))
	{
		fprintf(stderr, "[ERROR] Provided input file %s doesn't exist", pInputPath);
		return 1;
	}

	if (sfoFile.IsSFO(pInputPath))
	{
		printf("[INFO] Reading SFO...\n");
		if (!sfoFile.Read(pInputPath)) return 1;

		printf("[INFO] Writing XML...\n");
		if (!sfoFile.WriteXml(pOutputPath)) return 1;
	}
	else
	{
		printf("[INFO] Reading XML...\n");
		if (!sfoFile.ReadXml(pInputPath)) return 1;

		printf("[INFO] Writing SFO...\n");
		if (!sfoFile.Write(pOutputPath)) return 1;
	}
}