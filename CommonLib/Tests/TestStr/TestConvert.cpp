//#include "pch.h"
//#include "../../../CommonLib/str/StringEncoding.h"

#include <iostream>
#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/filesystem/File.h"



void TestConvert()
{

	std::string sAnsiText = "�������RRRRTTTTAAAAA";

	std::string sAnsiTextUtf8 = CommonLib::StringEncoding::str_a2utf8_safe(sAnsiText);


	std::wstring convertWideUniv = CommonLib::StringEncoding::str_univ_utf82w_safe(sAnsiTextUtf8);
	std::wstring convertWide = CommonLib::StringEncoding::str_utf82w_safe(sAnsiTextUtf8);

	std::string ansiConverttUtf8 = CommonLib::StringEncoding::str_w2utf8_safe(convertWide);
	std::string ansiConvertUnivtUtf8 = CommonLib::StringEncoding::str_w2utf8_safe(convertWide);



}