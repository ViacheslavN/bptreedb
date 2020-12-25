// TestBPTrees.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>


void TestBPTreeSet();
 
int main()
{

	std::shared_ptr<CommonLib::IlogWriter> pLogWriter(new CommonLib::CConsoleLogWriter());

	Log.InnitLog(std::shared_ptr<CommonLib::IlogLineFormat>(), pLogWriter);


	TestBPTreeSet(); 

	return 0;
}
 