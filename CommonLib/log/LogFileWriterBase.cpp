#include "stdafx.h"
#include "../filesystem/File.h"
#include "../filesystem/PathUtils.h"
#include "../filesystem/filesystem.h"
#include "../str/str.h"
#include "LogFileWriterBase.h"

namespace CommonLib
{
	CLogFileWriterBase::CLogFileWriterBase(size_t maxLogSize, std::shared_ptr<IlogRetention>& pLogRetention) : m_maxLogSize(maxLogSize), m_logRetention(pLogRetention), m_currSize(0)
	{

	}

	CLogFileWriterBase::~CLogFileWriterBase()
	{

	}

	void CLogFileWriterBase::Open(const std::string& path)
	{
		try
		{
			Close();

			m_file = file::CFileCreator::OpenFileA(path.c_str(),  file::ofmOpenAlways, file::arWrite, file::smRead, file::oftBinary);
			m_file->SetFileEnd();

			m_logName = CPathUtils::FindFileName(path);
			m_path = path;

			if (m_logRetention.get() != nullptr)
				m_logRetention->LogProcessing(GetPath(), GetName());

			m_currSize = GetSize();
		}
		catch (std::exception& ex)
		{
			CExcBase::RegenExc("Failed to open log file {0}", path, ex);
		}
		
	}

	void CLogFileWriterBase::Close()
	{
		if (m_file.get() == nullptr)
			return;

		m_file->CloseFile();
		m_file.reset();
	}

	const std::string& CLogFileWriterBase::GetPath() const
	{
		return m_path;
	}

	const std::string& CLogFileWriterBase::GetName() const
	{
		return m_logName;
	}

	size_t CLogFileWriterBase::GetSize() const
	{
		if (m_file.get() == nullptr)
			return 0;

		return m_file->GetFileSize();
	}

	
}