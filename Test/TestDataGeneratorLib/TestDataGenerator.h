#pragma once

template<class _TValue, class _TValueCreator, int nMaxElemenInBuffer = 100000>
class TDataGenerator
{
public:

	typedef _TValue TValue;
	typedef _TValueCreator TValueCreator;


	TDataGenerator(uint32_t nMaxElement,const wstr& fileName) : m_nMaxElement(nMaxElement), m_fileName(fileName)
	{
	
		m_valueCreator.reset(new TValueCreator(m_nMaxElement));
	}

	~TDataGenerator()
	{}



	struct SDataBuffer
	{

		typedef std::vector<TValue> TDataBuffer;
		TDataBuffer m_DataBuf;
		uint32_t m_posInData;

		uint32_t m_startIdx{ 0 };
		uint32_t m_endIdx{ 0 };

		bool IsPosInBuffer(uint32_t idx)
		{
			return idx >= m_startIdx && idx < m_endIdx;
		}

		void Resize(uint32_t nSize)
		{
			m_DataBuf.resize(nSize);
		}

		TValue GetValue(uint32_t idx)
		{
			return m_DataBuf[idx - m_startIdx];
		}
	};

	uint32_t GetHeaderSize() const
	{

		return sizeof(uint32_t) + TValueCreator::GetHeaderSize();
	}

	void Open()
	{

		if (!CommonLib::CFileUtils::IsFileExist(m_fileName))
			CreateFile();


		m_pDataFile = CommonLib::file::CFileCreator::OpenFile(m_fileName.c_str(), CommonLib::file::ofmOpenExisting, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);

		uint32_t nCount = 0;
		m_pDataFile->Read((byte_t*)&nCount, sizeof(nCount));
		if (nCount < m_nMaxElement)
		{
			m_pDataFile->CloseFile();
			CreateFile();
			m_pDataFile = CommonLib::file::CFileCreator::OpenFile(m_fileName.c_str(), CommonLib::file::ofmOpenExisting, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
			m_pDataFile->Read((byte_t*)&nCount, sizeof(nCount));
		}


		m_valueCreator->ReadHeader(m_pDataFile);
		m_dataBuffer.Resize(nMaxElemenInBuffer * m_valueCreator->GetValueSize());
	}

	void CreateFile()
	{
		
		CommonLib::file::TFilePtr pFile = CommonLib::file::CFileCreator::OpenFile(m_fileName.c_str(), CommonLib::file::ofmCreateAlways, CommonLib::file::arWrite, CommonLib::file::smNoMode, CommonLib::file::oftBinary);

		pFile->Write((byte_t*)&m_nMaxElement, sizeof(m_nMaxElement));
		m_valueCreator->WriteHeader(pFile);
		for (uint32_t i = 0;i < m_nMaxElement; ++i)
		{
			m_valueCreator->WriteValue(i, pFile);
		}

	}


	uint32_t GetSize() { return m_nMaxElement; }

	TValue GetValue(uint32_t idx)
	{
		if (!(idx < m_nMaxElement))
			throw CommonLib::CExcBase("TestDataGenerator out of range idx: %1, size: %2", idx, m_nMaxElement);
		
		
		if (m_dataBuffer.IsPosInBuffer(idx))
			return m_dataBuffer.GetValue(idx);

		uint32_t nCountElem = m_nMaxElement - idx;
		if (nCountElem > nMaxElemenInBuffer)
			nCountElem = nMaxElemenInBuffer;


		uint32_t nFilePos = ((idx * m_valueCreator->GetValueSize()) + GetHeaderSize());
		m_pDataFile->SetFilePos(nFilePos, CommonLib::soFromBegin);

		m_dataBuffer.m_startIdx = idx;
		m_dataBuffer.m_endIdx = m_dataBuffer.m_startIdx + nCountElem;
		m_dataBuffer.m_DataBuf.clear();

		for (size_t i = 0; i < nCountElem; ++i)
		{
			TValue value = m_valueCreator->ReadValue(m_pDataFile);
			m_dataBuffer.m_DataBuf.push_back(value);
		}


		return m_dataBuffer.GetValue(idx);
	}


private:
	uint32_t m_nMaxElement;
	wstr m_fileName;
	CommonLib::file::TFilePtr m_pDataFile;
	std::shared_ptr<TValueCreator> m_valueCreator;
	SDataBuffer m_dataBuffer;



};