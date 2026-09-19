#pragma once
#include "../alloc/alloc.h"
#include "../exception/exc_base.h"
#include "io.h"
#include "MemoryStreamBuffer.h"

namespace CommonLib
{
enum enSeekOffset
{
    soFromBegin,
    soFromCurrent,
    soFromEnd,
};

class IAlloc;
class IStream;


typedef std::shared_ptr<IStream> TStreamPtr;

class IStream
{
public:
	static bool IsBigEndian()
	{
		static const uint16_t word = 0xFF00;
		return *((uint8_t*)& word) != 0;

	}
};



class IMemoryStream 
{
public:
	virtual void AttachBuffer(byte_t* pBuffer, size_t nSize, bool bCopy = false) = 0;
	virtual void AttachBuffer(IMemStreamBufferPtr ptrBuffer) = 0;
	virtual byte_t* DeattachBuffer() = 0;

	virtual byte_t* Buffer() = 0;
	virtual const byte_t* Buffer() const = 0;
	virtual byte_t* BufferFromCurPos() = 0;
	virtual const byte_t* BufferFromCurPos() const = 0;

	virtual void Create(size_t nSize) = 0;
	virtual void Resize(size_t nSize) = 0;


	virtual void Seek(size_t position, enSeekOffset offset) = 0;
	virtual bool SeekSafe(size_t position, enSeekOffset offset) = 0;

	virtual void Reset() = 0;
	virtual bool IsEnoughSpace(size_t size) const = 0;
};


class IWriteStream : public io::IWrite
{
public:
	
	IWriteStream(){}
	virtual ~IWriteStream() {}

    virtual size_t Size() const = 0;
    virtual size_t Pos() const = 0;

	virtual std::streamsize WriteBytes(const byte_t* buffer, size_t size) = 0;
	virtual std::streamsize WriteInverse(const byte_t* buffer, size_t size) = 0;
	virtual std::streamsize WriteDataSafe(const byte_t* buffer, size_t size);


	template <typename T>
	void WriteT(const T& value)
	{
		Write((byte_t*)&value, sizeof(T));
	}

	template <typename T>
	bool WriteTSafe(const T& value)
	{	 
		return WriteDataSafe((byte_t*)&value, sizeof(T)) != 0;
	}

	virtual void Write(bool value);
	virtual void Write(uint8_t value);
	virtual void Write(char value);
	virtual void Write(int16_t value);
	virtual void Write(uint16_t value);
	virtual void Write(uint32_t value);
	virtual void Write(int32_t value);
	virtual void Write(int64_t value);
	virtual void Write(uint64_t value);
	virtual void Write(float value);
	virtual void Write(double value);
	virtual void Write(const std::string& str);
	virtual void Write(const std::wstring& str);
	virtual void Write(const char* pszStr);
	virtual void Write(const wchar_t* pszStr);
	virtual std::streamsize Write(const byte_t* dataPtr, size_t dataSize);

	virtual bool WriteSafe(const byte_t* pBuffer, size_t bufLen);
	virtual bool WriteSafe(bool value);
	virtual bool WriteSafe(uint8_t value);
	virtual bool WriteSafe(char value);
	virtual bool WriteSafe(int16_t value);
	virtual bool WriteSafe(uint16_t value);
	virtual bool WriteSafe(uint32_t value);
	virtual bool WriteSafe(int32_t value);
	virtual bool WriteSafe(int64_t value);
	virtual bool WriteSafe(uint64_t value);
	virtual bool WriteSafe(float value);
	virtual bool WriteSafe(double value);
	virtual bool WriteSafe(const std::string& str);
	virtual bool WriteSafe(const std::wstring& str);
};

typedef std::shared_ptr<IWriteStream> WriteStreamPtr;


class IReadStream : public io::IRead
{
public:

	virtual ~IReadStream() {}
	IReadStream() {}

	template <typename T>
	void ReadT(T& val)
	{
		Read((byte_t*)&val, sizeof(T));
	}


	template <typename T>
	T ReadTR()
	{
		T ret;
		Read((byte_t*)&ret, sizeof(T));

		return ret;
	}


	template <typename T>
	bool SafeReadT(T& val)
	{		 
		return ReadSafe((byte_t*)&val, sizeof(T)) != 0;
	}

	virtual std::streamsize ReadBytes(byte_t* dst, size_t size) = 0;
	virtual std::streamsize ReadInverse(byte_t* buffer, size_t size) = 0;
    virtual size_t Size() const = 0;
    virtual size_t Pos() const = 0;
	 


	virtual std::streamsize Read(byte_t* pBuffer, size_t bufLen);
	virtual void Read(bool& value);
	virtual void Read(char& value);
	virtual void Read(byte_t& value);
	virtual void Read(int16_t& value);
	virtual void Read(uint16_t& value);
	virtual void Read(uint32_t& value);
	virtual void Read(int32_t& value);
	virtual void Read(int64_t& value);
	virtual void Read(uint64_t& value);
	virtual void Read(float& value);
	virtual void Read(double& value);
	virtual void Read(std::string& str);
	virtual void Read(std::wstring& str);

	virtual std::streamsize ReadSafe(byte_t* pBuffer, uint32_t bufLen);
	virtual bool ReadSafe(bool& value);
	virtual bool ReadSafe(char& value);
	virtual bool ReadSafe(byte_t& value);
	virtual bool ReadSafe(int16_t& value);
	virtual bool ReadSafe(uint16_t& value);
	virtual bool ReadSafe(uint32_t& value);
	virtual bool ReadSafe(int32_t& value);
	virtual bool ReadSafe(int64_t& value);
	virtual bool ReadSafe(uint64_t& value);
	virtual bool ReadSafe(float& value);
	virtual bool ReadSafe(double& value);
	virtual bool ReadSafe(std::string& str);
	virtual bool ReadSafe(std::wstring& str);

	virtual bool         ReadBool();
	virtual byte_t       ReadByte();
	virtual char         ReadChar();
	virtual int16_t      Readint16();
	virtual uint16_t     Readintu16();
	virtual uint32_t     ReadDword();
	virtual int32_t      ReadInt32();
	virtual uint32_t     ReadIntu32();
	virtual int64_t      ReadInt64();
	virtual uint64_t     ReadIntu64();
	virtual float        ReadFloat();
	virtual double       ReadDouble();
	virtual std::string  ReadAstr();
	virtual std::wstring	     ReadWstr();

};

typedef std::shared_ptr<IReadStream> ReadStreamPtr;

class IMemoryWriteStream : public IWriteStream, public IMemoryStream
{
public:

	virtual std::streamsize WriteDataSafe(const byte_t* pBuffer, size_t bufLen);

	IMemoryWriteStream() {}
	virtual ~IMemoryWriteStream() {}
};


class IMemoryReadStream : public IReadStream, public IMemoryStream
{
public:
	virtual std::streamsize ReadSafe(byte_t* pBuffer, size_t bufLen);

	IMemoryReadStream() {}
	virtual ~IMemoryReadStream() {}
};

typedef std::shared_ptr<IMemoryWriteStream> IMemoryWriteStreamPtr;
typedef std::shared_ptr<IMemoryReadStream> IMemoryReadStreamPtr;


template<class I>
class TMemoryStreamBase : public I
{

private:
	TMemoryStreamBase(const TMemoryStreamBase& stream) {}
	TMemoryStreamBase& operator=(const TMemoryStreamBase& stream) {}
public:

	TMemoryStreamBase(std::shared_ptr<IAlloc> ptrAlloc = std::shared_ptr<IAlloc>()) :  m_nPos(0)
	{
		m_bIsBigEndian = IStream::IsBigEndian();
		m_ptrBuffer = std::make_shared<CMemoryStreamBuffer>(ptrAlloc);
	}

	~TMemoryStreamBase()
	{
	
	}

	//IStream
	virtual size_t Size() const
	{
		return m_ptrBuffer->GetSize();
	}

	virtual void Seek(size_t position, enSeekOffset offset)
	{
		try
		{
			if (!m_ptrBuffer->GetData())
				throw CExcBase(L"buffer is null", position);

			size_t newpos = 0;
			switch (offset)
			{
			case soFromBegin:
				newpos = position;
				break;
			case soFromCurrent:
				newpos = m_nPos + position;
				break;
			case soFromEnd:
				newpos = Size() - position;
				break;
			}	 

			if (newpos > Size())
			{
				Resize(newpos);
			}
			m_nPos = newpos;
		}
		catch (CExcBase& exc)
		{
			exc.AddMsgT(L"Can't seek stream pos: {0}", position);
			throw;
		}
	}

	bool SeekSafe(size_t position, enSeekOffset offset)
	{
		try
		{
			Seek( position,  offset);
		}
		catch (...)
		{
			return false;
		}

		return true;
	}
	
	virtual size_t Pos() const
	{
		return m_nPos;
	}
	
	virtual void Reset()
	{
		m_nPos = 0;
	}
	
	virtual void Close()
	{
		m_ptrBuffer->Close();
	}


	//IMemoryStream
	virtual void AttachBuffer(byte_t* pBuffer, size_t nSize, bool bCopy = false)
	{
		m_ptrBuffer->AttachBuffer(pBuffer, (uint32_t)nSize, bCopy);
	}

	virtual void AttachBuffer(IMemStreamBufferPtr ptrBuffer)
	{
		m_ptrBuffer = ptrBuffer;
	}

	virtual byte_t* DeattachBuffer()
	{ 
		m_nPos = 0; 
		return m_ptrBuffer->DeattachBuffer();
	}

	virtual byte_t* Buffer()
	{
		return m_ptrBuffer->GetData();
	}

	virtual const byte_t* Buffer() const
	{
		return m_ptrBuffer->GetData();
	}

	virtual byte_t* BufferFromCurPos()
	{
		return Buffer() + m_nPos;
	}

	virtual const byte_t* BufferFromCurPos() const
	{
		return Buffer() + m_nPos;
	}

	virtual void Create(size_t nSize)
	{
		try
		{
			m_nPos = 0;
			m_ptrBuffer->Create((uint32_t)nSize);
		}
		catch (CExcBase& exc)
		{
			exc.AddMsgT(L"Can't create stream size: {0}", nSize);
			throw;
		}
	}

	virtual void Resize(size_t nSize) { throw CExcBase(L"TMemoryStreamBase: resize isn't implemented"); }
protected:

	IMemStreamBufferPtr m_ptrBuffer;	 
	size_t  m_nPos; 
	bool m_bIsBigEndian; 
 
 };

 
}
