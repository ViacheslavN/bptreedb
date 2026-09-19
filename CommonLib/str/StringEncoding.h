#pragma once

namespace CommonLib
{
	namespace StringEncoding
	{
					   
		void str_utf82w(const std::string& utf8, std::wstring& unicode);
		void str_w2utf8(const std::wstring& unicode,  std::string& utf8);
        std::string str_w2utf8(const std::wstring& unicode);

		void str_utf82w_safe(const  std::string& utf8, std::wstring& unicode);
		void str_w2utf8_safe(const std::wstring& unicode,  std::string& utf8);

        std::wstring str_utf82w_safe(const  std::string& utf8);
        std::string str_w2utf8_safe(const std::wstring& unicode);


		void str_w2a(const std::wstring& src_str,  std::string& stra);
        std::string str_w2a(const std::wstring& unicode);
        std::string str_w2a_safe(const std::wstring& unicode);
		void str_a2w(const  std::string& src_str, std::wstring& unicode);
        std::wstring str_a2w(const  std::string& src_str);
        std::wstring str_a2w_safe(const  std::string& src_str);

		void str_univ_utf82w(const  std::string& srca, std::wstring& unicode);
		void str_univ_w2utf8(const std::wstring& unicode,  std::string& stra);

        std::wstring str_univ_utf82w(const  std::string& stra);
        std::string str_univ_w2utf8(const std::wstring& unicode);

        std::wstring str_univ_utf82w_safe(const  std::string& stra);
        std::string str_univ_w2utf8_safe(const std::wstring& unicode);


        std::string str_a2utf8(const  std::string& src_str);
        std::string str_a2utf8_safe(const  std::string& src_str);

	}

}