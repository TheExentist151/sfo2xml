#ifndef _HELPERS_H_INCLUDED_
#define _HELPERS_H_INCLUDED_

#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

namespace help
{
inline static bool FileExists(const char* pInputPath)
{
	DWORD attributes = GetFileAttributesA(pInputPath);
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

inline static char* GetPathWithoutExtension(const char* pInputPath)
{
	size_t stringLength = strlen(pInputPath);

	for (size_t i = 0; i < stringLength; i++)
	{
		if (pInputPath[stringLength - i] == '.')
		{
			size_t length = stringLength - i;

			char* pathWithoutExtension = new char[length + 1];
			memcpy(pathWithoutExtension, pInputPath, length);
			pathWithoutExtension[length] = 0x00;

			return pathWithoutExtension;
		}
	}

	return NULL;
}

inline static char* ChangeExtension(const char* pInputPath, const char* pExtension)
{
	const char* pathWithoutExtension = GetPathWithoutExtension(pInputPath);
	size_t pathLength = strlen(pathWithoutExtension);

	size_t extensionLength = strlen(pExtension);
	char* result = new char[pathLength + extensionLength + 2];

	memcpy(result, pathWithoutExtension, pathLength);
	memcpy(result + pathLength + 1, pExtension, extensionLength);
	result[pathLength] = '.';
	result[pathLength + extensionLength + 1] = 0x00;

	return result;
}

inline static char* GetExtension(const char* pInputPath)
{
	size_t stringLength = strlen(pInputPath);

	for (size_t i = 0; i < stringLength; i++)
	{
		if (pInputPath[stringLength - i] == '.')
		{
			size_t length = i;

			char* extension = new char[length + 1];
			memcpy(extension, pInputPath + (stringLength - i) + 1, length);
			extension[length] = 0x00;

			return extension;
		}
	}

	return NULL;
}
} // help

#endif // _HELPERS_H_INCLUDED_