#include "stdafx.h"
#include "convertToString.h"
#include "StringEncoding.h"
#include "../exception/exc_base.h"
#include <string.h>

namespace CommonLib
{
    std::wstring str_utils::WStrInt8(int8_t val)
	{
		return WStrInt32(int32_t(val));
	}

    std::wstring str_utils::WStrUint8(uint8_t val)
	{
		return WStrUint32(uint32_t(val));
	}

    std::wstring str_utils::WStrInt16(int16_t val)
	{
		return WStrInt32(int32_t(val));
	}

    std::wstring str_utils::WStrUint16(uint16_t val)
	{
		return WStrUint32(uint32_t(val));
	}

    std::wstring str_utils::WStrInt32(int32_t val)
	{
		wchar_t  buf[32];
#ifdef _WIN32
		swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"%d", val);
#else
		swprintf(buf, sizeof(buf) / sizeof(wchar_t), L"%d", val);
#endif
		return buf;
	}

    std::wstring str_utils::WStrUint32(uint32_t val)
	{
		wchar_t  buf[32];
#ifdef _WIN32
		swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"%u", val);
#else
		swprintf(buf, sizeof(buf) / sizeof(wchar_t), L"%u", val);
#endif
		return buf;
	}

    std::wstring str_utils::WStrInt64(int64_t val)
	{
		wchar_t  buf[32];
#ifdef _WIN32
		swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"%I64d", val);
#else
		swprintf(buf, sizeof(buf) / sizeof(wchar_t), L"%I64d", val);
#endif
		return buf;
	}

    std::wstring str_utils::WStrUint64(uint64_t val)
	{
		wchar_t  buf[32];
#ifdef _WIN32
		swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"%I64u", val);
#else
		swprintf(buf, sizeof(buf) / sizeof(wchar_t), L"%I64u", val);
#endif
		return buf;
	}

    std::wstring str_utils::WStrFloat(float val)
	{
		wchar_t  buf[32];
#ifdef _WIN32
		swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"%.3f", val);
#else
		swprintf(buf, sizeof(buf) / sizeof(wchar_t), L"%.3f", val);
#endif
		return buf;
	}

    std::wstring str_utils::WStrDouble(double val)
	{
		wchar_t buf[32];
#ifdef _WIN32
		swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"%.3lf", val);
#else
		swprintf(buf, sizeof(buf) / sizeof(wchar_t), L"%.3lf", val);
#endif
		return buf;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////

    std::string str_utils::AStrBool(bool val)
	{
		return val ? "1" : "0";
	}

    std::string str_utils::AStrInt8(int8_t val)
	{
		return AStrInt32(int32_t(val));
	}

    std::string str_utils::AStrUint8(uint8_t val)
	{
		return AStrUint32(uint32_t(val));
	}

    std::string str_utils::AStrInt16(int16_t val)
	{
		return AStrInt32(int32_t(val));
	}

    std::string str_utils::AStrUint16(uint16_t val)
	{
		return AStrUint32(uint32_t(val));
	}


    std::string str_utils::AStrInt8(int8_t val, uint32_t nZero)
	{
		return AStrInt32(int32_t(val), nZero);
	}

    std::string str_utils::AStrUint8(uint8_t val, uint32_t nZero)
	{
		return AStrUint32(uint32_t(val), nZero);
	}

    std::string str_utils::AStrInt16(int16_t val, uint32_t nZero)
	{
		return AStrInt32(int32_t(val), nZero);
	}

    std::string str_utils::AStrUint16(uint16_t val, uint32_t nZero)
	{
		return AStrUint32(uint32_t(val), nZero);
	}

    std::string str_utils::AStrInt32(int32_t val)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%d", val);
#else
		snprintf(buf, sizeof(buf), "%d", val);
#endif
		return buf;
	}

    std::string str_utils::AStrInt32(int32_t val, uint32_t nZero)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%0*d", nZero, val);
#else
		snprintf(buf, sizeof(buf), "%0*d", nZero, val);
#endif
		return buf;
	}

    std::string str_utils::AStrUint32(uint32_t val)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%u", val);
#else
		snprintf(buf, sizeof(buf), "%u", val);
#endif
		return buf;
	}

    std::string str_utils::AStrUint32(uint32_t val, uint32_t nZero)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%0*u", nZero, val);
#else
		snprintf(buf, sizeof(buf), "%0*u", nZero, val);
#endif
		return buf;
	}

    std::string str_utils::AStrInt64(int64_t val, uint32_t nZero)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%0*I64d", nZero, val);
#else
		snprintf(buf, sizeof(buf), "%0*lld", nZero, val);
#endif
		return buf;
	}

    std::string str_utils::AStrInt64(int64_t val)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%I64d", val);
#else
		snprintf(buf, sizeof(buf), "%I64d", val);
#endif
		return buf;
	}

    std::string str_utils::AStrUint64(uint64_t val)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%I64u", val);
#else
		snprintf(buf, sizeof(buf), "%I64du", val);
#endif
		return buf;
	}

    std::string str_utils::AStrUint64(uint64_t val, uint32_t nZero)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%0*I64u", nZero, val);
#else
		snprintf(buf, sizeof(buf), "%0*llu", val);
#endif
		return buf;
	}

    std::string str_utils::AStrFloat(float val)
	{
		char  buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%.3f", val);
#else
		snprintf(buf, sizeof(buf), "%.3f", val);
#endif
		return buf;
	}

    std::string str_utils::AStrDouble(double val)
	{
		char buf[32];
#ifdef _WIN32
		sprintf_s(buf, sizeof(buf), "%.3lf", val);
#else
		snprintf(buf, sizeof(buf), "%.3lf", val);
#endif
		return buf;
	}

    std::string str_utils::AStrFrom(const wchar_t *ptrStr)
	{
		return StringEncoding::str_w2utf8_safe(ptrStr);
	}

    std::string str_utils::AStrFrom(const std::wstring& val)
	{
		return StringEncoding::str_w2utf8_safe(val);
	}

	//================================================================================================


	int StrCaseIcmp(const std::string& str1, const std::string& str2)
	{
#ifdef  _WIN32
		return _stricmp(str1.c_str(), str2.c_str());
#else
		return strcasecmp(str1.c_str(), str2.c_str());
#endif
	}


	bool str_utils::AStr2Bool(const std::string& val)
	{
		if (StrCaseIcmp(val, "true"))
			return true;
		else if (StrCaseIcmp(val, "1"))
			return true;
		else if (StrCaseIcmp(val, "0"))
			return false;
		else if (StrCaseIcmp(val, "false"))
			return false;
	 
		throw CExcBase("Failed convert str to bool, {0}", val);
	}
	
	int8_t str_utils::AStr2Int8(const std::string& val)
	{
		return (int8_t)AStr2Int32(val);
	}

	uint8_t str_utils::AStr2Uint8(const std::string& val)
	{
		return (uint8_t)AStr2Uint32(val);
	}

	int16_t str_utils::AStr2Int16(const std::string& val)
	{
		return (int16_t)AStr2Int32(val);
	}

	uint16_t str_utils::AStr2Uint16(const std::string& val)
	{
		return (uint16_t)AStr2Uint32(val);
	}

	int32_t str_utils::AStr2Int32(const std::string& val)
	{
		const char  *start = val.c_str();
		char *stop = 0;

		int32_t ret = strtol(val.c_str(), &stop, 10);
		if (start == stop)
			throw CExcBase("Failed convert str to int32, {0}", val);

		return ret;
	}

	uint32_t str_utils::AStr2Uint32(const std::string& val)
	{
		const char  *start = val.c_str();
		char *stop = 0;

		uint32_t ret = strtoul(val.c_str(), &stop, 10);
		if (start == stop)
			throw CExcBase("Failed convert str to uint32_t, {0}", val);

		return ret;
	}

	int64_t str_utils::AStr2Int64(const std::string& val)
	{
		const char  *start = val.c_str();
		char *stop = 0;

		int64_t ret = strtoll(val.c_str(), &stop, 10);
		if (start == stop)
			throw CExcBase("Failed convert str to int64_t, {0}", val);

		return ret;
	}

	uint64_t str_utils::AStr2Uint64(const std::string& val)
	{
		const char  *start = val.c_str();
		char *stop = 0;

		uint64_t ret = strtoul(val.c_str(), &stop, 10);
		if (start == stop)
			throw CExcBase("Failed convert str to uint64_t, {0}", val);

		return ret;
	}

	float str_utils::AStr2Float(const std::string& val)
	{
		return (float)AStr2Double(val);
	}

	double str_utils::AStr2Double(const std::string& val)
	{
		const char  *start = val.c_str();
		char *stop = 0;

		double ret = strtod(val.c_str(), &stop);
		if (start == stop)
			throw CExcBase("Failed convert str to double, {0}", val);

		return ret;
	}

    template<>
    bool str_utils::AStr2TVal<bool>(const std::string& val)
    {
        return AStr2Bool(val);
    }

    template<>
    int8_t str_utils::AStr2TVal<int8_t>(const std::string& val)
    {
        return AStr2Int8(val);
    }

    template<>
    uint8_t str_utils::AStr2TVal<uint8_t>(const std::string& val)
    {
        return AStr2Uint8(val);
    }

    template<>
    int16_t str_utils::AStr2TVal<int16_t>(const std::string& val)
    {
        return AStr2Int16(val);
    }

    template<>
    uint16_t str_utils::AStr2TVal<uint16_t>(const std::string& val)
    {
        return AStr2Uint16(val);
    }

    template<>
    int32_t str_utils::AStr2TVal<int32_t>(const std::string& val)
    {
        return AStr2Int32(val);
    }

    template<>
    uint32_t str_utils::AStr2TVal<uint32_t>(const std::string& val)
    {
        return AStr2Uint32(val);
    }

    template<>
    int64_t str_utils::AStr2TVal<int64_t>(const std::string& val)
    {
        return AStr2Int64(val);
    }

    template<>
    uint64_t str_utils::AStr2TVal<uint64_t>(const std::string& val)
    {
        return AStr2Uint64(val);
    }

    template<>
    float str_utils::AStr2TVal<float>(const std::string& val)
    {
        return AStr2Float(val);
    }

    template<>
    double str_utils::AStr2TVal<double>(const std::string& val)
    {
        return AStr2Double(val);
    }

    template<>
    std::string str_utils::AStr2TVal<std::string>(const std::string& val)
    {
        return val;
    }


}