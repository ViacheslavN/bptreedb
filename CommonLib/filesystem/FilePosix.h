#pragma once

#include "File.h"

namespace CommonLib
{
	namespace file
	{
		namespace posix
		{
			class CFilePosix : public IFile
			{
			public:
				CFilePosix();
				~CFilePosix();

				virtual void Attach(FileHandle handle);
				virtual FileHandle Deattach();
				virtual FileHandle Handle() const;
				virtual void OpenFileA(const char *pszFileNameUtf8, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType);
				virtual void OpenFile(const wchar_t *pszFileName, enOpenFileMode mode, enAccesRights access, enShareMode share, enOpenFileType openType);
				virtual uint64_t GetFileSize() const;
				virtual void SetFilePos64(uint64_t nPos, enSeekOffset offset);
				virtual void SetFilePos(uint32_t nPos, enSeekOffset offset);
				virtual uint64_t GetFilePos() const;
				virtual void SetFileEnd();
				virtual void SetFileBegin();
				virtual std::streamsize Write(const byte_t* pData, size_t nSize);
				virtual std::streamsize Read(byte_t* pData, size_t nSize);
				virtual void CloseFile();
				virtual bool IsValid() const;
				virtual void Flush();
			private:

				FileHandle m_FileHandle{ -1 };
				bool m_Attach{ false };
			};
		}

		typedef posix::CFilePosix CFile;
	}
}