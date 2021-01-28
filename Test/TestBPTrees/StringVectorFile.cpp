#include "pch.h"
#include "StringVectorFile.h"


CStringVectorFile::CStringVectorFile(const wstr & fileName, uint32_t nMaxSize) : m_fileName(fileName), m_MaxSize(nMaxSize)
{

}

CStringVectorFile::~CStringVectorFile()
{

}

void CStringVectorFile::ReadData(std::vector<astr>& vec, uint32_t nSize)
{
	if (!CommonLib::CFileUtils::IsFileExist(m_fileName))
	{
		CreateFile(vec);
		return;
	}

	CommonLib::file::TFilePtr pFile = CommonLib::file::CFileCreator::OpenFile(m_fileName.c_str(), CommonLib::file::ofmOpenExisting, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
	uint32_t size = 0;

	pFile->Read((byte_t*)&size, sizeof(size));


	std::vector<byte_t> vecBuff;

	vecBuff.resize(255);

	if (size > nSize)
		size = nSize;


	for (size_t i = 0; i < size; ++i)
	{
		uint32_t len = 0;
		pFile->Read((byte_t*)&len, sizeof(len));
		pFile->Read((byte_t*)vecBuff.data(), len);
		astr str = (char*)vecBuff.data();

		vec.push_back(str);
	}



}

void CStringVectorFile::GetRandomValues(byte *pData, uint32_t nSize)
{
	if (!nSize)
		return;


	for (uint32_t i = 0; i < nSize; ++i)
	{
		uint32_t val = (rand() % 10 + 1);

		if (val > 9)
			pData[i] = byte_t(val) + 'a';
		else
			pData[i] = byte_t(val) + '0';
	}

}


void CStringVectorFile::CreateFile(std::vector<astr>& vecString)
{

	CommonLib::file::TFilePtr pFile = CommonLib::file::CFileCreator::OpenFile(m_fileName.c_str(), CommonLib::file::ofmCreateNew, CommonLib::file::arWrite, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
 
	char rndstr[rnd_size];
	memset(rndstr, 0, sizeof(rndstr));
	srand((int)time(0));

	for (uint32_t i = 0; i < m_MaxSize; ++i)
	{
		astr str = CommonLib::str_utils::AStrInt32(i);

		GetRandomValues((byte *)rndstr, sizeof(rndstr));

		rndstr[sizeof(rndstr) - 1] = '\0';

		str += rndstr;

		astr utf8 = CommonLib::StringEncoding::str_a2utf8(str);
		vecString.push_back(utf8);
	}

	pFile->Write((byte_t*)&m_MaxSize, sizeof(m_MaxSize));

	for (size_t i = 0; i < m_MaxSize; ++i)
	{
		uint32_t len = (uint32_t)vecString[i].length();
		pFile->Write((byte_t*)&len, sizeof(len));
		pFile->Write((byte_t*)vecString[i].c_str(), vecString[i].length());
	}

}