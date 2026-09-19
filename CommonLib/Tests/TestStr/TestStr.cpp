// TestStr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"


#include <iostream>
#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/str/str.h"

void TestConvert();

int main()
{

 

	//CommonLib::file::CFile *file;

	//TestConvert();

	uint8_t uint8 = 0xff;
	int32_t uint32 = 124563544;
	double dbl = 3.1 / 7;
	float  flt = 5.345 / 7;
	int64_t int64 = 0xffffffffeeee;
	uint64_t UINT64 = uint64_t(-1);


    std::wstring wtr64 = CommonLib::str_utils::WStrUint64(UINT64);

	wchar_t pFormat[] = L"{TestU8} {0}, Test int {1}, test double {2}, test float {3}, test int64 {4}, test uint64 {5} {}";

    std::string stra = CommonLib::str_format::AStrFormatSafeT("{TestU8} TestU8 {0}, Test int {1}, test double {2}, test float {3}, test int64 {4}, test uint64 {5} {}{{{{{",
		uint8, uint32, dbl, flt, int64, UINT64);

    std::wstring strw = CommonLib::str_format::WStrFormatSafeT(pFormat, uint8, uint32, dbl, flt, int64, UINT64);
    std::wstring strw1 = CommonLib::str_format::WStrFormatSafeT(L"{", uint8, uint32, dbl, flt, int64, UINT64);
	return 0;


}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
