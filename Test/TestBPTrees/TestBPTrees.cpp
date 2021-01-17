// TestBPTrees.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>


void TestBPTreeSet();
void TestBPTreeMap();
void TestBPTString();
 
int main()
{

	std::shared_ptr<CommonLib::IlogWriter> pLogWriter(new CommonLib::CConsoleLogWriter());

	Log.InnitLog(std::shared_ptr<CommonLib::IlogLineFormat>(), pLogWriter);


	//TestBPTreeSet(); 

	TestBPTreeMap();

	//TestBPTString();

	return 0;
}
 