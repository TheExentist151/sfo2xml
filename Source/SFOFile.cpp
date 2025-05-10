#include <cstdio>
#include <cstdlib>
#include <tinyxml2.h>

#include "SFOFile.h"

bool SFOFile::Read(const char* pInputPath)
{
	FILE* pInputFile = fopen(pInputPath, "rb");

	RawSFOHeader* pRawSFOHeader = new RawSFOHeader();
	fread(pRawSFOHeader, sizeof(RawSFOHeader), 1, pInputFile);

	if (pRawSFOHeader->Version != SFO_VERSION)
	{
		fprintf(stderr, "[ERROR] Provided %s SFO file's version is unknown\n", pInputPath);
		return false;
	}

	m_SFOEntries.reserve(pRawSFOHeader->EntryCount);

	for (uint32_t i = 0; i < pRawSFOHeader->EntryCount; i++)
	{
		RawSFOEntry* pRawSFOEntry = new RawSFOEntry();
		fread(pRawSFOEntry, sizeof(RawSFOEntry), 1, pInputFile);

		long nextEntry = ftell(pInputFile);

		SFOEntry entry = SFOEntry();
		entry.Format = pRawSFOEntry->Format;
		entry.DataLength = pRawSFOEntry->DataLength;
		entry.DataMaxLength = pRawSFOEntry->DataMaxLength;

		long absoluteKeyOffset = pRawSFOEntry->KeyOffset + pRawSFOHeader->KeyTableStart;
		fseek(pInputFile, absoluteKeyOffset, SEEK_SET);
		fgets(entry.Key, 32, pInputFile);

		long absoluteDataOffset = pRawSFOEntry->DataOffset + pRawSFOHeader->DataTableStart;
		fseek(pInputFile, absoluteDataOffset, SEEK_SET);
		entry.Data = new char[entry.DataLength + 1];
		fread(entry.Data, entry.DataLength, 1, pInputFile);

		// We don't really need to add null-terminator for int32_t and utf8 entries,
		// but utf8-s doesn't have a null terminator in the file, so we have to
		// add null-terminator here.
		entry.Data[entry.DataLength] = 0x00;

		m_SFOEntries.emplace_back(entry);
		fseek(pInputFile, nextEntry, SEEK_SET);
		delete pRawSFOEntry;
	}

	fclose(pInputFile);
	delete pRawSFOHeader;
	return true;
}

bool SFOFile::Write(const char* pOutputPath)
{
	FILE* pOutputFile = fopen(pOutputPath, "wb");
	if (!pOutputFile)
	{
		fprintf(stderr, "[ERROR] Can't open output file for writing");
		return false;
	}

	RawSFOHeader* pRawSFOHeader = new RawSFOHeader();
	pRawSFOHeader->Magic = SFO_MAGIC;
	pRawSFOHeader->Version = SFO_VERSION;
	pRawSFOHeader->EntryCount = (uint32_t)m_SFOEntries.size();
	pRawSFOHeader->DataTableStart = 0;
	pRawSFOHeader->KeyTableStart = 0;

	fwrite(pRawSFOHeader, sizeof(RawSFOHeader), 1, pOutputFile);

	// Writing entries
	for (size_t i = 0; i < m_SFOEntries.size(); i++)
	{
		RawSFOEntry* pRawSFOEntry = new RawSFOEntry();
		pRawSFOEntry->DataMaxLength = m_SFOEntries[i].DataMaxLength;
		pRawSFOEntry->Format = m_SFOEntries[i].Format;
		pRawSFOEntry->DataOffset = 0;
		pRawSFOEntry->KeyOffset = 0;

		switch (m_SFOEntries[i].Format)
		{
			case SFO_ENTRY_TYPE_INT32:
			{
				pRawSFOEntry->DataLength = 4;
				break;
			}
			case SFO_ENTRY_TYPE_UTF8: 
			case SFO_ENTRY_TYPE_UTF8_S:
			{
				pRawSFOEntry->DataLength = m_SFOEntries[i].DataLength;
				break;
			}
		}

		fwrite(pRawSFOEntry, sizeof(RawSFOEntry), 1, pOutputFile);
		delete pRawSFOEntry;
	}

	// Writing keys
	uint32_t keyTableOffset = ftell(pOutputFile);
	fseek(pOutputFile, 0x8, SEEK_SET);
	fwrite(&keyTableOffset, 4, 1, pOutputFile);
	fseek(pOutputFile, keyTableOffset, SEEK_SET);

	for (size_t i = 0; i < m_SFOEntries.size(); i++)
	{
		uint32_t realKeyOffset = ftell(pOutputFile);
		uint16_t keyOffset = realKeyOffset - keyTableOffset;

		fseek(pOutputFile, (long)(0x14 + (i * 0x10)), SEEK_SET);
		fwrite(&keyOffset, 2, 1, pOutputFile);
		fseek(pOutputFile, realKeyOffset, SEEK_SET);

		fwrite(m_SFOEntries[i].Key, strlen(m_SFOEntries[i].Key) + 1, 1, pOutputFile);
	}

	WritePadding(pOutputFile);

	// Writing data
	uint32_t dataTableOffset = ftell(pOutputFile);
	fseek(pOutputFile, 0xC, SEEK_SET);
	fwrite(&dataTableOffset, 4, 1, pOutputFile);
	fseek(pOutputFile, dataTableOffset, SEEK_SET);

	for (size_t i = 0; i < m_SFOEntries.size(); i++)
	{
		uint32_t realDataOffset = ftell(pOutputFile);
		uint16_t dataOffset = realDataOffset - dataTableOffset;

		fseek(pOutputFile, (long)(0x14 + ((i * 0x10) + 0xC)), SEEK_SET);
		fwrite(&dataOffset, 2, 1, pOutputFile);
		fseek(pOutputFile, realDataOffset, SEEK_SET);

		if (m_SFOEntries[i].Format == SFO_ENTRY_TYPE_INT32)
		{
			int32_t data = atoi(m_SFOEntries[i].Data);
			fwrite(&data, 4, 1, pOutputFile);
		}
		else
		{
			char* data = new char[m_SFOEntries[i].DataMaxLength + 1];

			for (size_t y = 0; y < m_SFOEntries[i].DataMaxLength; y++)
				data[y] = 0x00;

			memcpy(data, m_SFOEntries[i].Data, m_SFOEntries[i].DataLength);
			fwrite(data, m_SFOEntries[i].DataMaxLength, 1, pOutputFile);
		}

		WritePadding(pOutputFile);
	}

	return true;
}

bool SFOFile::ReadXml(const char* pInputPath)
{
	tinyxml2::XMLDocument document;
	document.LoadFile(pInputPath);
	if (document.Error())
	{
		fprintf(stderr, "[ERROR] An error occurred while trying to load the input file: %s (line number %d)\n", 
			document.ErrorName(), document.ErrorLineNum());
		return false;
	}

	tinyxml2::XMLElement* pRootElement = document.FirstChildElement("sfo");
	if (!pRootElement)
	{
		fprintf(stderr, "[ERROR] Provided XML file doesn't have a root \"sfo\" element\n");
		return false;
	}

	tinyxml2::XMLElement* pParamElement = pRootElement->FirstChildElement("sfoparam");

	while (pParamElement)
	{
		SFOEntry sfoEntry;
		const char* key = pParamElement->FindAttribute("key")->Value();
		memcpy(sfoEntry.Key, key, strlen(key) + 1);

		const char* data = pParamElement->GetText();
		if (data)
		{
			sfoEntry.Data = new char[strlen(data) + 1];
			memcpy(sfoEntry.Data, data, strlen(data) + 1);
		}
		else
		{
			sfoEntry.Data = new char{ 0 };
		}

		sfoEntry.DataLength = (uint32_t)strlen(sfoEntry.Data);
		sfoEntry.DataMaxLength = pParamElement->FindAttribute("maxlen")->IntValue();

		const char* format = pParamElement->FindAttribute("fmt")->Value();
		if (strcmp(format, "int32") == 0)
		{
			sfoEntry.Format = SFO_ENTRY_TYPE_INT32;
		}
		else if (strcmp(format, "utf8") == 0)
		{
			sfoEntry.Format = SFO_ENTRY_TYPE_UTF8;

			// Adding 1 to the DataLength because of null-terminator
			sfoEntry.DataLength += 1;
		}
		else if (strcmp(format, "utf8-s") == 0)
		{
			sfoEntry.Format = SFO_ENTRY_TYPE_UTF8_S;
		}
		else
		{
			fprintf(stderr, "[ERROR] Unknown format in param %s\n", sfoEntry.Key);
			return false;
		}

		m_SFOEntries.push_back(sfoEntry);

		pParamElement = pParamElement->NextSiblingElement();
	}

	return true;
}

bool SFOFile::WriteXml(const char* pOutputPath)
{
	FILE* pOutputFile = fopen(pOutputPath, "w");
	if (!pOutputFile)
	{
		fprintf(stderr, "[ERROR] Can't open output file for writing\n");
		return false;
	}

	tinyxml2::XMLPrinter printer(pOutputFile);
	printer.OpenElement("sfo");

	for (size_t i = 0; i < m_SFOEntries.size(); i++)
	{
		printer.OpenElement("sfoparam");
		printer.PushAttribute("key", m_SFOEntries[i].Key);
		printer.PushAttribute("maxlen", m_SFOEntries[i].DataMaxLength);

		switch (m_SFOEntries[i].Format)
		{
			case SFO_ENTRY_TYPE_INT32:
			{
				printer.PushAttribute("fmt", "int32");
				
				int32_t value = 0;
				memcpy(&value, m_SFOEntries[i].Data, 4);

				printer.PushText(value);
				break;
			}
			case SFO_ENTRY_TYPE_UTF8:
			{
				printer.PushAttribute("fmt", "utf8");
				printer.PushText(m_SFOEntries[i].Data);
				break;
			}
			case SFO_ENTRY_TYPE_UTF8_S:
			{
				printer.PushAttribute("fmt", "utf8-s");
				printer.PushText(m_SFOEntries[i].Data);
				break;
			}
		}
		printer.CloseElement();
	}

	printer.CloseElement();
	fclose(pOutputFile);

	return true;
}

bool SFOFile::IsSFO(const char* pInputPath)
{
	FILE* pInputFile = fopen(pInputPath, "rb");

	uint32_t magic = 0;
	fread(&magic, 4, 1, pInputFile);

	if (magic != SFO_MAGIC) return false;

	fclose(pInputFile);

	return true;
}