#include "stdafx.h"
#include "FileAsyncLogger.h"
#include "filesystem/File.h"
#include "filesystem/PathUtils.h"
#include "filesystem/filesystem.h"
#include "synch/thread.h"

namespace CommonLib
{
 

	CFileAsyncLogger::CFileAsyncLogger(size_t maxLogSize, std::shared_ptr<IlogRetention>& pLogRetention) : CLogFileWriterBase(maxLogSize, pLogRetention)
	{
        std::string threadName = "log:";
		m_thread.reset(new synch::CThread(
			
			[this]() 
			{
				ThreadFunc();
			}, "log:"));
	}

	CFileAsyncLogger::~CFileAsyncLogger()
	{
		m_queue.AbortQueue();

		if (m_thread.get() != nullptr)
			m_thread->Join();

	}

	std::shared_ptr<CommonLib::IlogWriter> CFileAsyncLogger::Create(const std::string& path, size_t maxLogSize, std::shared_ptr<IlogRetention>& pLogRetention)
	{
		std::shared_ptr<CommonLib::IlogWriter> fileLogger(new CommonLib::CFileAsyncLogger(maxLogSize, pLogRetention));
		((CommonLib::CFileAsyncLogger*)fileLogger.get())->Open(path);

		return fileLogger;
	}

	void CFileAsyncLogger::Write(const std::string& msg)
	{
		m_queue.Push(msg);
	}

	void CFileAsyncLogger::ChangeCurrentLogFile()
	{

		try
		{

            std::string path = GetPath();

            std::string logName = CPathUtils::FindOnlyFileName(path);
            std::string logPath = CPathUtils::FindFilePath(path);
            std::string logExt = CPathUtils::FindFileExtension(path);

            std::string fileLogName;
			for (uint32_t i = 0; i < 0xFFFFFFFF; ++i)
			{
				fileLogName = str_format::AStrFormatSafeT("{0}\\{1}_{2}.{3}", logPath, logName, i, logExt);
				if (!CFileUtils::IsFileExist(fileLogName))
					break;
			}

			CLogFileWriterBase::Close();
			CFileUtils::RenameFile(path, fileLogName);
			CLogFileWriterBase::Open(path);

			if (m_logRetention.get() != nullptr)
				m_logRetention->ArcLogFile(fileLogName);

		}
		catch (std::exception& exc)
		{
			exc;
			//TO DO log
		}
	}


	void CFileAsyncLogger::ThreadFunc()
	{
		try
		{

			while (true)
			{
                std::string msg = m_queue.Pop();
				
				if (msg.empty() && m_queue.IsAborted())
				{
					break;
				}

				WriteMessage(msg);
			}

		}
		catch (std::exception& srcExc)
		{
			srcExc;
		}
	}

	void CFileAsyncLogger::WriteMessage(const std::string& msg)
	{
		if (m_file.get() == nullptr)
			return;

		m_currSize += msg.size();

		if (m_currSize > m_maxLogSize)
		{
			ChangeCurrentLogFile();
			m_currSize = 0;
		}

		m_file->Write((const byte_t*)msg.c_str(), (uint32_t)msg.size());
		
	}
}