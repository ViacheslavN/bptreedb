#include "stdafx.h"
#include "str.h"

namespace CommonLib
{

template <class TChar>
long TStrtol(const TChar * str, TChar ** endptr, int base)
{

}

template<>
long TStrtol<wchar_t>(const wchar_t * str, wchar_t ** endptr, int base)
{
	return  wcstol(str, endptr, base);
}


template<>
long TStrtol<char>(const char * str, char ** endptr, int base)
{
	return  strtol(str, endptr, base);
}


template <class TChar, class TStr, class TContainer>
TStr StrFormatSafeT(const TStr& format, const TContainer& args, TChar startSymbol, TChar endSymbol)
{
	TStr result;
	for (size_t i = 0, sz = format.length(); i < sz;)
	{
		if (format[i] == startSymbol)
		{
			size_t pos = format.find(endSymbol, i);
			if (pos == TStr::npos)
			{
				result += format.c_str() + i;
				break;
			}

			size_t len = pos - i - 1;
			TStr subStr = format.substr(i + 1, len);

			TChar* pSubStrAfterNumber = 0;
			long  arg = TStrtol<TChar>(subStr.c_str(), &pSubStrAfterNumber, 10);
			if (subStr.c_str() == pSubStrAfterNumber)
			{
				arg = -1;
			}		 

			if (arg >= (long)args.size() || arg < 0)
			{
				result += startSymbol;
				result += subStr;
				result += endSymbol;
			}
			else
				result += args[arg];

			i = pos + 1;

		}
		else
		{
			result += format[i];
			i += 1;
		}

	}

	return result;
}

    std::wstring str_format::StrFormatSafe(const std::wstring& str)
{
	return str;
}

    std::wstring str_format::StrFormatSafe(const std::wstring& format, const std::vector<std::wstring>& args)
{
	return StrFormatSafeT(format, args, L'{', L'}');
}

    std::wstring str_format::StrFormatSafe(const std::wstring& format, const std::wstring& arg1)
{
    std::vector<std::wstring>  args;
	args.push_back(arg1);

	return StrFormatSafe(format, args);
}

    std::wstring str_format::StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2)
{
    std::vector<std::wstring>  args;
	args.push_back(arg1);
	args.push_back(arg2);

	return StrFormatSafe(format, args);
}

    std::wstring str_format::StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3)
{
    std::vector<std::wstring>  args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);

	return StrFormatSafe(format, args);
}

    std::wstring str_format::StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4)
{
    std::vector<std::wstring> args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	args.push_back(arg4);

	return StrFormatSafe(format, args);
}

    std::wstring str_format::StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4, const std::wstring& arg5)
{
    std::vector<std::wstring>  args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	args.push_back(arg4);
	args.push_back(arg5);

	return StrFormatSafe(format, args);
}

    std::wstring str_format::StrFormatSafe(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4, const std::wstring& arg5, const std::wstring& arg6)
{
    std::vector<std::wstring>  args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	args.push_back(arg4);
	args.push_back(arg5);
	args.push_back(arg6);

	return StrFormatSafe(format, args);
}
//////////////////////////////////////////////////////////////////////////////////////


std::string str_format::StrFormatSafe(const std::string& format, const std::vector<std::string >& args)
{
	return StrFormatSafeT(format, args, '{', '}');
}

std::string str_format::StrFormatSafe(const std::string& str)
{
	return str;
}

std::string str_format::StrFormatSafe(const std::string& format, const std::string& arg1)
{
    std::vector<std::string >  args;
	args.push_back(arg1);

	return StrFormatSafe(format, args);
}

std::string str_format::StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2)
{
    std::vector<std::string >  args;
	args.push_back(arg1);
	args.push_back(arg2);

	return StrFormatSafe(format, args);
}

    std::string str_format::StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2, const std::string& arg3)
{
    std::vector<std::string >  args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);

	return StrFormatSafe(format, args);
}

    std::string str_format::StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2, const std::string& arg3, const std::string& arg4)
{
    std::vector<std::string >  args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	args.push_back(arg4);

	return StrFormatSafe(format, args);
}

    std::string str_format::StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2, const std::string& arg3, const std::string& arg4, const std::string& arg5)
{
    std::vector<std::string >  args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	args.push_back(arg4);
	args.push_back(arg5);

	return StrFormatSafe(format, args);
}


    std::string str_format::StrFormatSafe(const std::string& format, const std::string& arg1, const std::string& arg2, const std::string& arg3, const std::string& arg4, const std::string& arg5, const std::string& arg6)
{
    std::vector<std::string >  args;
	args.push_back(arg1);
	args.push_back(arg2);
	args.push_back(arg3);
	args.push_back(arg4);
	args.push_back(arg5);
	args.push_back(arg6);

	return StrFormatSafe(format, args);
}


    std::string str_format::StrFormatAnySafe(const std::any& value)
{

    if (auto x = std::any_cast<bool>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<int8_t>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<uint8_t>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<int16_t>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<uint16_t>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<int32_t>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<uint32_t>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<int64_t>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<uint64_t>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<float>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<double>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<std::string>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    if (auto x = std::any_cast<const char*>(&value)) {
        return str_utils::AStrFrom(*x);
    }

    throw CExcBase("Converting any  to string failed, Unknown type any");
}


}