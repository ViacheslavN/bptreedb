#include <iostream>
#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/filesystem/filesystem.h"
#include "../../../CommonLib/compress/zlib/ZlibUtil.h"

void TestZlib()
{
	{
		CommonLib::file::TFilePtr srcFile = CommonLib::file::CFileCreator::OpenFile(L"D:\\6.mp4", CommonLib::file::ofmOpenAlways, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
		CommonLib::file::TFilePtr dstFile = CommonLib::file::CFileCreator::OpenFile(L"D:\\6.mp4.zl", CommonLib::file::ofmCreateAlways, CommonLib::file::arWrite, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
		CommonLib::CZLibUtil::CompressData(CommonLib::CZLibUtil::BestCompression, srcFile, dstFile);
	}

	{
		CommonLib::file::TFilePtr srcFile = CommonLib::file::CFileCreator::OpenFile(L"D:\\6.mp4.zl", CommonLib::file::ofmOpenAlways, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
		CommonLib::file::TFilePtr dstFile = CommonLib::file::CFileCreator::OpenFile(L"D:\\6_new.mp4", CommonLib::file::ofmCreateAlways, CommonLib::file::arWrite, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
		CommonLib::CZLibUtil::DecompressData(srcFile, dstFile);
	}
 
}