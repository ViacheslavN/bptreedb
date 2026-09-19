// TestStr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"


#include <iostream>
#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/Variant/Variant.h"
#include "../../../CommonLib/data/blob.h"
#include "../../../CommonLib/Variant/VariantVisitor.h"

void TestConvert();

int main()
{


    bool bcycle = false;
    while (bcycle)
    {
        ::Sleep(10);
    }

    int dd = 10;
    std::string sVal = "1234566788";
    std::wstring swVal = L"1234566788";



     CommonLib::CVariant varInt(dd);
     CommonLib::CVariant varStr(sVal);
     CommonLib::CVariant varWStr(swVal);

    CommonLib::CStringVisitor stringVisitor;



      int dd1 = varInt.Get<int>();
      std::string sVal1 = varStr.Get<std::string>();
      std::wstring swVal1 = varWStr.Get<std::wstring>();

    CommonLib::ApplyVisitor(varInt, stringVisitor);

      uint32_t size = 10;
      CommonLib::Data::CBlobPtr ptrBlob = std::make_shared< CommonLib::Data::CBlob>(0);
      for(uint32_t i = 0; i <size; ++i)
          ptrBlob->Push_back(0);

      CommonLib::CVariant varptrBlob(ptrBlob);

      CommonLib::Data::CBlobPtr ptrBlob1 = varptrBlob.Get<CommonLib::Data::CBlobPtr>();

    CommonLib::CVariant varptrBlob1 = varptrBlob;
    CommonLib::Data::CBlobPtr ptrBlob2 = varptrBlob.Get<CommonLib::Data::CBlobPtr>();



    varptrBlob = dd1;
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
