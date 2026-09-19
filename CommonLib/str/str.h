#pragma once
#include "convertToString.h"
#include <any>

namespace CommonLib
{

class str_format
{
public:

	static std::wstring StrFormatSafe(const std::wstring& format, const std::vector<std::wstring>& args);
		
	static std::wstring StrFormatSafe(const std::wstring& str);
	static std::wstring StrFormatSafe(const std::wstring& format, const std::wstring& arg1);
	static std::wstring StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2);
	static std::wstring StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3);
	static std::wstring StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4);
	static std::wstring StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4, const std::wstring& arg5);
	static std::wstring StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4, const std::wstring& arg5, const std::wstring& arg6);

	template<class TArg>
	static std::wstring WStrFormatSafeT(const std::wstring& format, const TArg& arg1)
	{
		return StrFormatSafe(format, str_utils::WStrFrom(arg1));
	}

	template<class TArg1, class TArg2>
	static std::wstring WStrFormatSafeT(const std::wstring& format, const TArg1& arg1, const TArg2& arg2)
	{
		return StrFormatSafe(format, str_utils::WStrFrom(arg1), str_utils::WStrFrom(arg2));
	}

	template<class TArg1, class TArg2, class TArg3>
	static std::wstring WStrFormatSafeT(const std::wstring& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
	{
		return StrFormatSafe(format, str_utils::WStrFrom(arg1), str_utils::WStrFrom(arg2), str_utils::WStrFrom(arg3));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	static std::wstring WStrFormatSafeT(const std::wstring& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
	{
		return StrFormatSafe(format, str_utils::WStrFrom(arg1), str_utils::WStrFrom(arg2), str_utils::WStrFrom(arg3), str_utils::WStrFrom(arg4));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
	static std::wstring WStrFormatSafeT(const std::wstring& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4, const TArg5& arg5)
	{
		return StrFormatSafe(format, str_utils::WStrFrom(arg1), str_utils::WStrFrom(arg2), str_utils::WStrFrom(arg3), str_utils::WStrFrom(arg4), str_utils::WStrFrom(arg5));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5, class TArg6>
	static std::wstring WStrFormatSafeT(const std::wstring& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4, const TArg5& arg5, const TArg6& arg6)
	{
		return StrFormatSafe(format, str_utils::WStrFrom(arg1), str_utils::WStrFrom(arg2), str_utils::WStrFrom(arg3), str_utils::WStrFrom(arg4), str_utils::WStrFrom(arg5), str_utils::WStrFrom(arg6));
	}

	static std::string StrFormatSafe(const std::string& format, const std::vector<std::string >& args);

	static std::string StrFormatSafe(const std::string& str);
	static std::string StrFormatSafe(const std::string& format, const std::string& arg1);
	static std::string StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2);
	static std::string StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2, const std::string& arg3);
	static std::string StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2, const std::string& arg3, const std::string& arg4);
	static std::string StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2, const std::string& arg3, const std::string& arg4, const std::string& arg5);
	static std::string StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2, const std::string& arg3, const std::string& arg4, const std::string& arg5, const std::string& arg6);
    static std::string StrFormatAnySafe(const std::any& val);

	template<class TArg>
	static std::string AStrFormatSafeT(const std::string& format, const TArg& arg1)
	{
		return StrFormatSafe(format, str_utils::AStrFrom(arg1));
	}

	template<class TArg1, class TArg2>
	static std::string AStrFormatSafeT(const std::string& format, const TArg1& arg1, const TArg2& arg2)
	{
		return StrFormatSafe(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2));
	}

	template<class TArg1, class TArg2, class TArg3>
	static std::string AStrFormatSafeT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
	{
		return StrFormatSafe(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	static std::string AStrFormatSafeT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
	{
		return StrFormatSafe(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
	static std::string AStrFormatSafeT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4, const TArg5& arg5)
	{
		return StrFormatSafe(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4), str_utils::AStrFrom(arg5));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5, class TArg6>
	static std::string AStrFormatSafeT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4, const TArg5& arg5, const TArg6& arg6)
	{
		return StrFormatSafe(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4), str_utils::AStrFrom(arg5), str_utils::AStrFrom(arg6));
	}

};
	
}