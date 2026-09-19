#pragma once

namespace CommonLib
{

	class str_utils
	{
	public:
		static std::wstring WStrInt8(int8_t val);
		static std::wstring WStrUint8(uint8_t val);
		static std::wstring WStrInt16(int16_t val);
		static std::wstring WStrUint16(uint16_t val);
		static std::wstring WStrInt32(int32_t val);
		static std::wstring WStrUint32(uint32_t val);
		static std::wstring WStrInt64(int64_t val);
		static std::wstring WStrUint64(uint64_t val);
		static std::wstring WStrFloat(float val);
		static std::wstring WStrDouble(double val);


		static std::wstring WStrFrom(const wchar_t *ptrStr)
		{
			return std::wstring(ptrStr);
		}

		static std::wstring WStrFrom(const std::wstring& val)
		{
			return val;
		}

		static  std::wstring WStrFrom(const int8_t& val)
		{
			return WStrInt8(val);
		}


		static  std::wstring WStrFrom(const uint8_t& val)
		{
			return WStrUint8(val);
		}


		static  std::wstring WStrFrom(const int16_t& val)
		{
			return WStrInt16(val);
		}

		static  std::wstring WStrFrom(const uint16_t& val)
		{
			return WStrUint16(val);
		}


		static  std::wstring WStrFrom(const int32_t& val)
		{
			return WStrInt32(val);
		}


		static  std::wstring WStrFrom(const uint32_t& val)
		{
			return WStrUint32(val);
		}


		static  std::wstring WStrFrom(const int64_t& val)
		{
			return WStrInt64(val);
		}


		static  std::wstring WStrFrom(const uint64_t& val)
		{
			return WStrUint64(val);
		}


		static std::wstring WStrFrom(const float& val)
		{
			return WStrFloat(val);
		}

		static  std::wstring WStrFrom(const double& val)
		{
			return WStrDouble(val);
		}

		static std::string AStrBool(bool val);
		static std::string AStrInt8(int8_t val);
		static std::string AStrUint8(uint8_t val);
		static std::string AStrInt16(int16_t val);
		static std::string AStrUint16(uint16_t val);
		static std::string AStrInt32(int32_t val);
		static std::string AStrUint32(uint32_t val);
		static std::string AStrInt64(int64_t val);
		static std::string AStrUint64(uint64_t val);

		static std::string AStrInt8(int8_t val, uint32_t nZero);
		static std::string AStrUint8(uint8_t val, uint32_t nZero);
		static std::string AStrInt16(int16_t val, uint32_t nZero);
		static std::string AStrUint16(uint16_t val, uint32_t nZero);
		static std::string AStrInt32(int32_t val, uint32_t nZero);
		static std::string AStrUint32(uint32_t val, uint32_t nZero);
		static std::string AStrInt64(int64_t val, uint32_t nZero);
		static std::string AStrUint64(uint64_t val, uint32_t nZero);



		static std::string AStrFloat(float val);
		static std::string AStrDouble(double val);
		static std::string AStrFrom(const wchar_t *ptrStr);
		static std::string AStrFrom(const std::wstring& val);


		static std::string AStrFrom(const std::string& val)
		{
			return val;
		}

		static std::string AStrFrom(bool val)
		{
			return AStrBool(val);
		}

		static  std::string AStrFrom(const int8_t& val)
		{
			return AStrInt8(val);
		}

		static  std::string AStrFrom(const uint8_t& val)
		{
			return AStrUint8(val);
		}


		static  std::string AStrFrom(const int16_t& val)
		{
			return AStrInt16(val);
		}

		static  std::string AStrFrom(const uint16_t& val)
		{
			return AStrUint16(val);
		}

		static  std::string AStrFrom(const int32_t& val)
		{
			return AStrInt32(val);
		}

		static  std::string AStrFrom(const int32_t& val, uint32_t nZero)
		{
			return AStrInt32(val, nZero);
		}

		static  std::string AStrFrom(const uint32_t& val)
		{
			return AStrUint32(val);
		}

		static  std::string AStrFrom(const uint32_t& val, uint32_t nZero)
		{
			return AStrUint32(val, nZero);
		}

		static  std::string AStrFrom(const int64_t& val)
		{
			return AStrInt64(val);
		}

		static  std::string AStrFrom(const uint64_t& val)
		{
			return AStrUint64(val);
		}

		static std::string AStrFrom(const float& val)
		{
			return AStrFloat(val);
		}

		static  std::string AStrFrom(const double& val)
		{
			return AStrDouble(val);
		}

		static bool AStr2Bool(const std::string& val);
		static int8_t AStr2Int8(const std::string& val);
		static uint8_t AStr2Uint8(const std::string& val);
		static int16_t AStr2Int16(const std::string& val);
		static uint16_t AStr2Uint16(const std::string& val);
		static int32_t AStr2Int32(const std::string& val);
		static uint32_t AStr2Uint32(const std::string& val);
		static int64_t AStr2Int64(const std::string& val);
		static uint64_t AStr2Uint64(const std::string& val);
		static float AStr2Float(const std::string& val);
		static double AStr2Double(const std::string& val);


		template<class T>
		static T AStr2TVal(const std::string& val);
	};




}