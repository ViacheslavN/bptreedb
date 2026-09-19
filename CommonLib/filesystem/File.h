#pragma once

#include "../stream/stream.h"
#include "../stream/io.h"

namespace CommonLib
{
	namespace file
	{
		enum enAccesRights
		{
			arExecute = 0x00000001,
			arRead = 0x00000002,
			arWrite = 0x00000004,
			aeReadWrite = arRead | arWrite
		};

		enum enShareMode
		{
			smNoMode = 0x00,
			smDelete = 0x00000001,
			smRead = 0x00000002,
			smWrite = 0x00000004,
		};

		enum enOpenFileMode
		{
			ofmCreateAlways,
			ofmCreateNew,
			ofmOpenAlways,
			ofmOpenExisting,
		};

		enum enOpenFileType
		{
			oftBinary,
			oftText
		};

#ifdef _WIN32
		typedef   HANDLE  FileHandle;
#elif __linux__
		typedef   int  FileHandle;
#endif

		class IFile;
		typedef std::shared_ptr<IFile> TFilePtr;

		class IFile : public io::IWrite, public io::IRead
		{
		public:
			IFile() {};
			virtual ~IFile() {};
			virtual void Attach(FileHandle handle) = 0;
			virtual FileHandle Deattach() = 0;
			virtual FileHandle Handle() const = 0;
			virtual void OpenFileA(const char *pszFileNameUtf8, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType) = 0;
			virtual void OpenFile(const wchar_t *pszFileName, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType) = 0;
			virtual uint64_t GetFileSize() const = 0;
			virtual void SetFilePos64(uint64_t nPos, enSeekOffset offset) = 0;
			virtual void SetFilePos(uint32_t nPos, enSeekOffset offset) = 0;
			virtual uint64_t GetFilePos() const = 0;
			virtual void SetFileEnd() = 0;
			virtual void SetFileBegin() = 0;
			virtual std::streamsize Write(const  byte_t* pData, size_t nSize) = 0;
			virtual std::streamsize Read(byte_t* pData, size_t nSize) = 0;
			virtual void CloseFile() = 0;
			virtual bool IsValid() const = 0;
			virtual void Flush() = 0;
		};

		class CFileCreator
		{
		public:
			static TFilePtr OpenFileA(const char *pszFileNameUtf8, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType);
			static TFilePtr OpenFile(const wchar_t *pszFileName,  enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType);
		};
	}
}