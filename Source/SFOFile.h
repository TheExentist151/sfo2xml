#ifndef _SFOFILE_H_INCLUDED_
#define _SFOFILE_H_INCLUDED_

#include <cstdint>
#include <vector>

struct RawSFOHeader
{
	uint32_t Magic;
	uint32_t Version;
	uint32_t KeyTableStart;
	uint32_t DataTableStart;
	uint32_t EntryCount;
};

struct RawSFOEntry
{
	uint16_t KeyOffset;
	uint16_t Format;
	uint32_t DataLength;
	uint32_t DataMaxLength;
	uint32_t DataOffset;
};

struct SFOEntry
{
	char* Key = new char[32];
	uint16_t Format = 0;
	uint32_t DataLength = 0;
	uint32_t DataMaxLength = 0;
	char* Data = NULL;
};

class SFOFile
{
public: 
	bool Read(const char* pInputPath);
	bool Write(const char* pOutputPath);

	bool ReadXml(const char* pInputPath);
	bool WriteXml(const char* pOutputPath);

	bool IsSFO(const char* pInputPath);

public: 
	std::vector<SFOEntry> m_SFOEntries;

private: 
	inline void WritePadding(FILE* pInputFile)
	{
		while (true)
		{
			long position = ftell(pInputFile);
			if (position % 4 != 0)
			{
				char paddingChar = 0x00;
				fwrite(&paddingChar, 1, 1, pInputFile);
			}
			else break;
		}
	}

private: 
	static const uint32_t SFO_MAGIC = 0x46535000;
	static const uint32_t SFO_VERSION = 0x00000101;

	static const uint16_t SFO_ENTRY_TYPE_UTF8_S = 0x0004;
	static const uint16_t SFO_ENTRY_TYPE_UTF8 = 0x0204;
	static const uint16_t SFO_ENTRY_TYPE_INT32 = 0x0404;
};

#endif // _SFOFILE_H_INCLUDED_