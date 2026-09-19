// TestLog.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include "CommonLib.h"
#include "str/str.h"
#include "filesystem/filesystem.h"
#include "log/logger.h"
#include "log/FileAsyncLogger.h"
#include "log/LogInfo.h"
#include "date/DateUtil.h"

void TestZlib();
#include "../../../CommonLib/exception/PosixExc.h"
int main()
{
/*	TestZlib();
	return 0;
	std::string path = "C:\\dir1\\dir2\\file.ttt\\";
	wstr pathw = L"C:\\dir1\\dir2\\file.ttt\\";


	std::string left = CommonLib::StringUtils::Left(path, 3);
	std::string right = CommonLib::StringUtils::Right(path, 3);

	std::string FileExtension = CommonLib::CPathUtils::FindFileExtension(path);
	std::string FileName = CommonLib::CPathUtils::FindFileName(path);
	std::string OnlyFileName = CommonLib::CPathUtils::FindOnlyFileName(path);
	std::string Path = CommonLib::CPathUtils::FindFilePath(path);

	wstr FileExtensionW = CommonLib::CPathUtils::FindFileExtension(pathw);
	wstr FileNameW = CommonLib::CPathUtils::FindFileName(pathw);
	wstr OnlyFileNameW = CommonLib::CPathUtils::FindOnlyFileName(pathw);
	wstr PathW = CommonLib::CPathUtils::FindFilePath(pathw);*/

	/*bool bCycle = true;
	while (bCycle)
	{
		sleep(1);
	}*/

	uint64_t dateTime = CommonLib::dateutil::CDateUtil::GetCurrentDateTime();

	CommonLib::CPosixExc posizExc(12, "{0}, {1}", 34, 45.5);

	std::string msg = posizExc.GetErrorMessageA(45);

	std::shared_ptr<CommonLib::IlogLineFormat> plogLineFormat;
	std::shared_ptr<CommonLib::IlogRetention> plogRetention;

	std::shared_ptr<CommonLib::IlogWriter> fileLogger = CommonLib::CFileAsyncLogger::Create("D:\\backup.log", 10000000, plogRetention);

	//printf("Width trick: %0*I64d \n", 30, (__int64)10 * 10000000);

	//CommonLib::CLogger log;

	Log.InnitLog(plogLineFormat, fileLogger);

	for (size_t i = 0; i < 100; ++i)
	{
		Log.InfoT("Message {0}", i);
		CommonLib::CLogInfo logInfo(Log, "{0}, {1}, {2}", i, i + 1, 3.154);
	//	logInfo.Ok();
	}

	/*fileLogger->Close();
	for (size_t i = 0; i < 100; ++i)
	{
		log.InfoT("Message {0}", i);
	}

	fileLogger->Open("D:\\backup.log");*/

#ifdef _WIN32
	::Sleep(2000);
#else
	sleep(2);
#endif
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
