#include "stdafx.h"
#include "StrUtils.h"
#include <ctype.h>
#include <algorithm>

namespace CommonLib
{

	std::string StringUtils::Left(const std::string& str, size_t pos)
	{

		if (str.length() < pos || str.empty())
			return {};

		return str.substr(0, pos);
	}

	std::string StringUtils::Right(const std::string& str, size_t pos)
	{
		if (str.length() < pos || str.empty())
			return {};

		size_t count = str.length() - pos;
		return str.substr(pos, count);
	}

	std::wstring StringUtils::Left(const std::wstring& str, size_t pos)
	{
		if (str.length() < pos || str.empty())
			return {};

		return str.substr(0, pos);
	}

	std::wstring StringUtils::Right(const std::wstring& str, size_t pos)
	{
		if (str.length() < pos || str.empty())
			return {};

		size_t count = str.length() - pos;
		return str.substr(pos, count);
	}


    /**
     * @brief Remove invalid UTF-8 characters from the string by setting to zero characters first bytes.
     * Algorithm - http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
     * @param[out] data - input string.
     * @param size - string size.
     * @return returns false in case there was a modification, otherwise true.
     */
    bool StringUtils::ValidateAndFixUtf8String(char* data, size_t size)
    {
        bool retVal(true);
        if (!size)
        {
            return retVal;
        }
        uint32_t octetCount(0);
        uint32_t octetCountBac(0);
        uint32_t codepoint(0);
        size_t i(0);
        size_t startSequence(0);

        enum State
        {
            Exit,
            NewSymbol,
            SearchFirstSymbol,
            SymbolsSequence,
            CheckSequence,
            ErrorSymbol,
        };
        State state(SearchFirstSymbol);
        State returnState(Exit);

        while (state != Exit)
        {
            switch (state)
            {
                case Exit:
                    break;
                case NewSymbol:
                    ++i;
                    if (i == size)
                    {
                        // end of line
                        if (returnState != SearchFirstSymbol)
                        {
                            state = ErrorSymbol;
                            returnState = Exit;
                            --i;
                        }
                        else
                        {
                            state = Exit;
                        }
                    }
                    else
                    {
                        state = returnState;
                    }
                    break;
                case ErrorSymbol:
                    // replace invalid character with '?'
                    while (startSequence <= i)
                    {
                        data[startSequence] = '?';
                        ++startSequence;
                    }
                    retVal = false;
                    state = returnState;
                    break;
                case SearchFirstSymbol:
                    // in the first byte of the sequence [at lease 2 most significant bits] on [none] must be set
                    startSequence = i;
                    // find the character length
                    state = NewSymbol;
                    if ((data[i] & 0x80) == 0)
                    { // 0xxx xxxx
                        returnState = SearchFirstSymbol;
                    }
                    else if ((data[i] & 0xE0) == 0xC0)
                    { //110x xxxx  10xx xxxx
                        octetCount = 1; // the first octet has already been processed
                        returnState = SymbolsSequence;
                    }
                    else if ((data[i] & 0xF0) == 0xE0)
                    { // 1110 xxxx  10xx xxxx  10xx xxxx
                        octetCount = 2;
                        returnState = SymbolsSequence;
                    }
                    else if ((data[i] & 0xF8) == 0xF0)
                    { // 1111 0xxx  10xx xxxx  10xx xxxx  10xx xxxx
                        octetCount = 3;
                        returnState = SymbolsSequence;
                    }
                    else
                    {
                        state = ErrorSymbol;
                        returnState = SearchFirstSymbol;
                    }
                    octetCountBac = octetCount;
                    break;
                case SymbolsSequence:
                {
                    // each byte in the sequence must start with 10xxxxxx
                    unsigned char res = data[i] & 0xC0; // 1100 0000
                    if (res != 0x80)
                    {
                        state = ErrorSymbol;
                        returnState = SearchFirstSymbol;
                    }
                    else
                    {
                        if (--octetCount)
                        {
                            state = NewSymbol;
                            returnState = SymbolsSequence;
                        }
                        else
                        {
                            // the sequence ended
                            state = CheckSequence;
                            returnState = SearchFirstSymbol;
                        }
                    }
                }
                    break;
                case CheckSequence:
                    // check the character validity
                    state = NewSymbol;
                    switch (octetCountBac)
                    {
                        case 1:
                            codepoint = ((data[startSequence] & 0x1F) << 6) | (data[startSequence + 1] & 0x3F);
                            if (codepoint < 0x80)
                            {
                                state = ErrorSymbol;
                            }
                            break;
                        case 2:
                            codepoint = ((data[startSequence] & 0x0F) << 12) | ((data[startSequence + 1] & 0x3F) << 6)
                                        | (data[startSequence + 2] & 0x3F);
                            if (codepoint < 0x0800 || (codepoint >= 0xD800 && codepoint <= 0xDFFF) || codepoint >= 0xFFFE)
                            {
                                state = ErrorSymbol;
                            }
                            break;
                        case 3:
                            codepoint = ((data[startSequence] & 0x07) << 18) | ((data[startSequence + 1] & 0x3F) << 12)
                                        | ((data[startSequence + 2] & 0x3F) << 6) | (data[startSequence + 3] & 0x3F);
                            if (codepoint < 0x10000 || codepoint > 0x10FFFF)
                            {
                                state = ErrorSymbol;
                            }
                            break;
                        default:
                            state = ErrorSymbol;
                    } // switch (octetCountBac)
                    break;
            } // switch
        } // while
        return retVal;
    }

    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    bool is_base64(unsigned char c)
    {
        return (std::isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string StringUtils::ToBase64(const std::string& str)
    {
        const char * bytes_to_encode = str.c_str();
        size_t in_len = str.length();

        std::string ret;
        int i = 0;
        int j = 0;
        char char_array_3[3];
        char char_array_4[4];

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; (i <4) ; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while((i++ < 3))
                ret += '=';

        }

        return ret;
    }

    void StringUtils::FromBase64(const std::string& str, std::string & buffer )
    {
        buffer.clear();

        int in_len = static_cast<int>(str.size());
        int i = 0;
        int j = 0;
        int in_idx = 0;
        char char_array_4[4], char_array_3[3];

        while (in_len-- && ( str[in_idx] != '=') && is_base64(str[in_idx])) {
            char_array_4[i++] = str[in_idx]; in_idx++;
            if (i ==4) {
                for (i = 0; i <4; i++)
                    char_array_4[i] = static_cast<char>(base64_chars.find(char_array_4[i]));

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    buffer.push_back(char_array_3[i]);
                i = 0;
            }
        }

        if (i) {
            for (j = i; j <4; j++)
                char_array_4[j] = 0;

            for (j = 0; j <4; j++)
                char_array_4[j] = static_cast<char>(base64_chars.find(char_array_4[j]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++)
                buffer.push_back(char_array_3[j]);
        }
    }

    std::vector<std::string> StringUtils::ArgsStringToArgsVector(const std::string& args)
    {
        enum ParseState
        {
            Reading, SpaceSymbol, OneQutingSymbol, DoubleQuotinsSymbol
        };

        std::vector<std::string> result;

        std::string  arg;
        ParseState state = Reading;
        for (int i = 0; i < args.length(); ++i)
        {
            const char ch = args[i];
            switch (ch)
            {
                case ' ':
                    switch(state)
                    {
                        case Reading:
                            if(!arg.empty())
                            {
                                result.push_back(arg);
                            }
                            arg.clear();
                            state = SpaceSymbol;
                            break;
                        case SpaceSymbol:
                              break;
                        case OneQutingSymbol:
                            arg += ch;
                            break;
                        case DoubleQuotinsSymbol:
                            arg += ch;
                            break;
                    }
                    break;
                case '\'':
                    switch (state)
                    {
                        case Reading:
                            arg += ch;
                            state = OneQutingSymbol;
                            break;
                        case SpaceSymbol:
                            arg += ch;
                            state = OneQutingSymbol;
                            break;
                        case OneQutingSymbol:
                            arg += ch;
                            state = Reading;
                            break;
                        case DoubleQuotinsSymbol:
                            arg += ch;
                            break;
                    }
                    break;
                case '"':
                    switch (state)
                    {
                        case Reading:
                            arg += ch;
                            state = DoubleQuotinsSymbol;
                            break;
                        case SpaceSymbol:
                            arg += ch;
                            state = DoubleQuotinsSymbol;
                            break;
                        case OneQutingSymbol:
                            arg += ch;
                            break;
                        case DoubleQuotinsSymbol:
                            arg += ch;
                            state = Reading;
                            break;
                    }
                    break;
                default:
                    switch (state)
                    {
                        case Reading:
                            arg += ch;
                            break;
                        case SpaceSymbol:
                            arg += ch;
                            state = Reading;
                            break;
                        case OneQutingSymbol:
                            arg += ch;
                            break;
                        case DoubleQuotinsSymbol:
                            arg += ch;
                            break;
                    }
                    break;
            }
        }

        if(!arg.empty())
        {
            if((state == Reading) || (state == SpaceSymbol))
            {
                result.push_back(arg);
            }
            else
            {
                throw CExcBase("Incorrect argument string");
            }
        }

        return  result;
    }


    bool StringUtils::Equals(const std::string& left, const std::string& right, bool caseSensitive )
    {
        return Compare(left, right, caseSensitive) == 0;
    }

    int  StringUtils::Compare(const std::string& left, const std::string& right, bool caseSensitive )
    {
        if(right.empty())
            return left.empty() ? 0 : 1;

        if(left.empty())
            return right.empty() ? 0 : -1;

        if(caseSensitive)
            return strcmp(left.c_str(), right.c_str());
        else
        {
            const char * pLeftStr = left.c_str();
            const char * pRightStr = right.c_str();
            for(int c1, c2; *pLeftStr && *pRightStr; ++pLeftStr, ++pRightStr)
            {
                c1 = (int)std::toupper(*pLeftStr);
                c2 = (int)std::toupper(*pRightStr);
                if(c1 != c2)
                    return c1 - c2;
            }
            return ((int)*pLeftStr) - ((int)*pRightStr);
        }
    }

    bool StringUtils::Equals(const std::wstring& left, const std::wstring& right, bool caseSensitive )
    {
        return Compare(left, right, caseSensitive) == 0;
    }

    int  StringUtils::Compare(const std::wstring& left, const std::wstring& right, bool caseSensitive)
    {
        if(right.empty())
            return left.empty() ? 0 : 1;

        if(left.empty())
            return right.empty() ? 0 : -1;

        if(caseSensitive)
            return wcscmp(left.c_str(), right.c_str());
        else {
            const wchar_t *pLeftStr = left.c_str();
            const wchar_t *pRightStr = right.c_str();
            for (int c1, c2; *pLeftStr && *pRightStr; ++pLeftStr, ++pRightStr) {
                c1 = (int) std::toupper(*pLeftStr);
                c2 = (int) std::toupper(*pRightStr);
                if (c1 != c2)
                    return c1 - c2;
            }
            return ((int) *pLeftStr) - ((int) *pRightStr);
        }
    }

    std::string StringUtils::Mid(const std::string& str, size_t first, size_t count)
    {
        uint32_t len = (uint32_t)str.length();

        if(count == std::string::npos)
            count = len - first;

        if(first == 0 && count >= len)
            return str;

        if(first >= len)
            return str;

        std::string result;
        if(first + count > len)
            count = len - first;

        result.reserve(count + 1);
        char* p = result.data();

        strncpy(p, str.c_str() + first, len);
        p[len] = 0;

        return result;
    }

    void StringUtils::Lower(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c){ return std::tolower(c); });
    }
}