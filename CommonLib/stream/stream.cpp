#include "stdafx.h"
#include "stream.h"

namespace CommonLib
{

	std::streamsize  IWriteStream::Write(const byte_t* pBuffer, size_t bufLen)
	{
		if (IStream::IsBigEndian())
			return WriteInverse(pBuffer, bufLen);
 
		return WriteBytes(pBuffer, bufLen);

	}

	void IWriteStream::Write(bool value)
	{
		WriteT<byte_t>(value ? (byte_t)1 : (byte_t)0);
	}

	void IWriteStream::Write(byte_t value)
	{
		WriteT<byte_t>(value);
	}

	void IWriteStream::Write(char value)
	{
		WriteT<char>(value);
	}

	void IWriteStream::Write(int16_t value)
	{
		WriteT<int16_t>(value);
	}

	void IWriteStream::Write(uint16_t value)
	{
		WriteT<uint16_t>(value);
	}

	void IWriteStream::Write(uint32_t value)
	{
		WriteT<uint32_t>(value);
	}

	void IWriteStream::Write(int32_t value)
	{
		WriteT<int32_t>(value);
	}

	void IWriteStream::Write(int64_t value)
	{
		WriteT<int64_t>(value);
	}
	void IWriteStream::Write(uint64_t value)
	{
		WriteT<uint64_t>(value);
	}

	void IWriteStream::Write(float value)
	{
		WriteT<float>(value);
	}

	void IWriteStream::Write(double value)
	{
		WriteT<double>(value);
	}
	 
	void IWriteStream::Write(const std::string& str)
	{
		uint32_t length = (uint32_t)str.length();
		Write(length);
		Write((byte_t*)str.c_str(), length);
	}
	void IWriteStream::Write(const std::wstring& str)
	{
		uint32_t length = (uint32_t)str.length();
		Write(length);
		Write((byte_t*)str.c_str(), sizeof(wchar_t) * length);
	}

	void IWriteStream::Write(const char* pszStr)
	{
		Write((byte_t*)pszStr, (uint32_t)strlen(pszStr));
	}

	void IWriteStream::Write(const wchar_t* pszStr)
	{
		Write((byte_t*)pszStr, 2 * (uint32_t)wcslen(pszStr));
	}

	std::streamsize IWriteStream::WriteDataSafe(const byte_t* pBuffer, size_t bufLen)
	{
 
		try
		{
			if (IStream::IsBigEndian())
				return WriteInverse(pBuffer, bufLen);

			return	WriteBytes(pBuffer, bufLen);

		}
 		catch (...)
		{
			return 0;
		}		
	}

	bool  IWriteStream::WriteSafe(const byte_t* pBuffer, size_t bufLen)
	{
		return WriteDataSafe(pBuffer, bufLen) != 0;
	}

	bool IWriteStream::WriteSafe(bool value)
	{
		return WriteTSafe<bool>(value);
	}

	bool IWriteStream::WriteSafe(uint8_t value)
	{
		return WriteTSafe<uint8_t>(value);
	}

	bool IWriteStream::WriteSafe(char value)
	{
		return WriteTSafe<char>(value);
	}

	bool IWriteStream::WriteSafe(int16_t value)
	{
		return WriteTSafe<int16_t>(value);
	}

	bool IWriteStream::WriteSafe(uint16_t value)
	{
		return WriteTSafe<uint16_t>(value);
	}

	bool IWriteStream::WriteSafe(uint32_t value)
	{
		return WriteTSafe<uint32_t>(value);
	}

	bool IWriteStream::WriteSafe(int32_t value)
	{
		return WriteTSafe<int32_t>(value);
	}

	bool IWriteStream::WriteSafe(int64_t value)
	{
		return WriteTSafe<int64_t>(value);
	}

	bool IWriteStream::WriteSafe(uint64_t value)
	{
		return WriteTSafe<uint64_t>(value);
	}

	bool IWriteStream::WriteSafe(float value)
	{
		return WriteTSafe<float>(value);
	}

	bool IWriteStream::WriteSafe(double value)
	{
		return WriteTSafe<double>(value);
	}

	bool IWriteStream::WriteSafe(const std::string& str)
	{
		uint32_t length = (uint32_t)str.length();
		
		if (WriteTSafe(length))
			return false;

		return WriteSafe((byte_t*)str.c_str(), length) != 0;

	}

	bool IWriteStream::WriteSafe(const std::wstring& str)
	{
		uint32_t length = (uint32_t)str.length();

	
		if (WriteTSafe(length))
		 return false;

		return WriteSafe((byte_t*)str.c_str(), sizeof(wchar_t) * length) != 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	std::streamsize IMemoryWriteStream::WriteDataSafe(const byte_t* pBuffer, size_t bufLen)
	{

		try
		{
			if (!IsEnoughSpace(bufLen))
				return 0;

			return IWriteStream::WriteDataSafe(pBuffer, bufLen);
		}
		catch (...)
		{
			return 0;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////

	std::streamsize IReadStream::Read(byte_t* pBuffer, size_t bufLen)
	{
		if (IStream::IsBigEndian())
			ReadInverse(pBuffer, bufLen);
		else
			ReadBytes(pBuffer, bufLen);

		return bufLen;
	}

	void IReadStream::Read(bool& value)
	{
		byte_t ret;
		ReadT<byte_t>(ret);
		value = (ret == 1) ? true : false;
	}

	void IReadStream::Read(char& value)
	{
		ReadT<char>(value);
	}

	void IReadStream::Read(byte_t& value)
	{
		ReadT<byte_t>(value);
	}

	void IReadStream::Read(int16_t& value)
	{
		ReadT<int16_t>(value);
	}

	void IReadStream::Read(uint16_t& value)
	{
		ReadT<uint16_t>(value);
	}

	void IReadStream::Read(uint32_t& value)
	{
		ReadT<uint32_t>(value);
	}

	void IReadStream::Read(int32_t& value)
	{
		ReadT<int32_t>(value);
	}

	void IReadStream::Read(int64_t& value)
	{
		ReadT<int64_t>(value);
	}

	void IReadStream::Read(uint64_t& value)
	{
		ReadT<uint64_t>(value);
	}

	void IReadStream::Read(float& value)
	{
		ReadT<float>(value);
	}

	void IReadStream::Read(double& value)
	{
		ReadT<double>(value);
	}

	void IReadStream::Read(std::string& str)
	{
		uint32_t nLen = ReadIntu32();
		if (nLen != 0)
		{
			str.resize(nLen);
			Read((byte_t*)str.data(), sizeof(char)*nLen);
		}
	}

	void IReadStream::Read(std::wstring& str)
	{
		uint32_t nLen = ReadIntu32();
		if (nLen != 0)
		{
			str.resize(nLen);
			Read((byte_t*)str.data(), sizeof(wchar_t)*nLen);
		}
	}

	std::streamsize IReadStream::ReadSafe(byte_t* pBuffer, uint32_t bufLen)
	{
		try
		{
			if (IStream::IsBigEndian())
				return ReadInverse((byte_t*)&pBuffer, bufLen);
	
			return ReadBytes((byte_t*)&pBuffer, bufLen);
		}
		catch (...)
		{
			return 0;
		}
	}

	bool IReadStream::ReadSafe(bool& value)
	{
		return SafeReadT<bool>(value);
	}

	bool IReadStream::ReadSafe(char& value)
	{
		return SafeReadT<char>(value);
	}

	bool IReadStream::ReadSafe(byte_t& value)
	{
		return SafeReadT<byte_t>(value);
	}

	bool IReadStream::ReadSafe(int16_t& value)
	{
		return SafeReadT<int16_t>(value);
	}

	bool IReadStream::ReadSafe(uint16_t& value)
	{
		return SafeReadT<uint16_t>(value);
	}

	bool IReadStream::ReadSafe(uint32_t& value)
	{
		return SafeReadT<uint32_t>(value);
	}

	bool IReadStream::ReadSafe(int32_t& value)
	{
		return SafeReadT<int32_t>(value);
	}

	bool IReadStream::ReadSafe(int64_t& value)
	{
		return SafeReadT<int64_t>(value);
	}

	bool IReadStream::ReadSafe(uint64_t& value)
	{
		return SafeReadT<uint64_t>(value);
	}

	bool IReadStream::ReadSafe(float& value)
	{
		return SafeReadT<float>(value);
	}

	bool IReadStream::ReadSafe(double& value)
	{
		return SafeReadT<double>(value);
	}

	bool IReadStream::ReadSafe(std::string& str)
	{
		uint32_t nLen = 0;
		if (!ReadSafe(nLen))
			return false;

		if (nLen == 0)
			return true;

		str.resize(nLen);

		return ReadSafe((byte_t*)&str[0], sizeof(char)*nLen);
	}

	bool IReadStream::ReadSafe(std::wstring& str)
	{
		uint32_t nLen = 0;
		if (!ReadSafe(nLen))
			return false;

		if (nLen == 0)
			return true;

		str.resize(nLen);

		return ReadSafe((byte_t*)&str[0], sizeof(wchar_t)*nLen);
	}

	bool IReadStream::ReadBool()
	{
		return (ReadTR<byte_t>() == 1) ? true : false;
	}

	byte_t IReadStream::ReadByte()
	{
		return ReadTR<byte_t>();
	}

	char IReadStream::ReadChar()
	{
		return ReadTR<char>();
	}

	int16_t IReadStream::Readint16()
	{
		return ReadTR<int16_t>();
	}

	uint16_t IReadStream::Readintu16()
	{
		return ReadTR<uint16_t>();
	}

	uint32_t IReadStream::ReadDword()
	{
		return ReadTR<uint32_t>();
	}

	int32_t  IReadStream::ReadInt32()
	{
		return ReadTR<int32_t>();
	}

	uint32_t IReadStream::ReadIntu32()
	{
		return ReadTR<uint32_t>();
	}

	int64_t IReadStream::ReadInt64()
	{
		return ReadTR<int64_t>();
	}

	uint64_t IReadStream::ReadIntu64()
	{
		return ReadTR<uint64_t>();
	}

	float  IReadStream::ReadFloat()
	{
		return ReadTR<float>();
	}

	double IReadStream::ReadDouble()
	{
		return ReadTR<double>();
	}

    std::string IReadStream::ReadAstr()
	{
        std::string str;
		Read(str);

		return str;
	}

    std::wstring IReadStream::ReadWstr()
	{
        std::wstring str;
		Read(str);

		return str;
	}

	///////////////////////////////////////////////////////////////////////////////////

	std::streamsize IMemoryReadStream::ReadSafe( byte_t* pBuffer, size_t bufLen)
	{

		try
		{
			if (!IsEnoughSpace(bufLen))
				return 0;

			return IReadStream::ReadSafe(pBuffer, (uint32_t)bufLen);
		}
		catch (...)
		{
			return 0;
		}
	}

}