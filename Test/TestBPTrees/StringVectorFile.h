#pragma once

class CStringVectorFile
{
public:

	CStringVectorFile(const wstr & fileName, uint32_t nMaxSize);
	~CStringVectorFile();


	void ReadData(std::vector<astr>& vec, uint32_t nSize);
private:
	wstr m_fileName;
	uint32_t m_MaxSize;
	void CreateFile(std::vector<astr>& vec);
	void GetRandomValues(byte *pData, uint32_t nSize);

	static const int rnd_size = 15;
};