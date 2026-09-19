#include "stdafx.h"
#include "FileStream.h"


namespace CommonLib
{

	CFileReadStream::CFileReadStream(file::TFilePtr ptrFile) : TBase(ptrFile)
	{}

	CFileReadStream::~CFileReadStream()
	{}

	//Stream
	size_t CFileReadStream::Size() const
	{
		return TBase::Size();
	}

	void CFileReadStream::Seek(size_t position, enSeekOffset offset)
	{
		TBase::Seek(position, offset);
	}

	bool CFileReadStream::SeekSafe(size_t position, enSeekOffset offset)
	{
		return TBase::SeekSafe(position, offset);
	}

	size_t CFileReadStream::Pos() const
	{
		return TBase::Pos();
	}

	void CFileReadStream::Reset()
	{
		TBase::Reset();
	}

	void CFileReadStream::Close()
	{
		TBase::Close();
	}


	std::streamsize CFileReadStream::ReadBytes(byte_t* dst, size_t size)
	{
		std::streamsize readSize = m_ptrFile->Read(dst, size);
		if(readSize != size)
			throw CExcBase("FileReadStream bytes to read {0}, result {1}", size, readSize);

		return readSize;
	}

	std::streamsize CFileReadStream::ReadInverse(byte_t* buffer, size_t size)
	{
		std::streamsize readSize = m_ptrFile->Read(buffer, size); //TO DO inverse
		if (readSize != size)
			throw CExcBase("FileReadStream bytes to read {0}, result {1}", size, readSize);

		return readSize;
	} 
	

	CFileWriteStream::CFileWriteStream(file::TFilePtr ptrFile) : TBase(ptrFile)
	{}

	CFileWriteStream::~CFileWriteStream()
	{}

	//Stream
	size_t CFileWriteStream::Size() const
	{
		return TBase::Size();
	}

	void CFileWriteStream::Seek(size_t position, enSeekOffset offset)
	{
		TBase::Seek(position, offset);
	}

	bool CFileWriteStream::SeekSafe(size_t position, enSeekOffset offset)
	{
		return TBase::SeekSafe(position, offset);
	}

	size_t CFileWriteStream::Pos() const
	{
		return TBase::Pos();
	}

	void CFileWriteStream::Reset()
	{
		TBase::Reset();
	}

	void CFileWriteStream::Close()
	{
		TBase::Close();
	}	

	std::streamsize CFileWriteStream::WriteBytes(const byte_t* buffer, size_t size)
	{
		return this->m_ptrFile->Write(buffer, size);
	}

	std::streamsize CFileWriteStream::WriteInverse(const byte_t* buffer, size_t size)
	{
		return this->m_ptrFile->Write(buffer, size); //TO DO do inverse
	}	

 
}