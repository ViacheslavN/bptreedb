#pragma once

#include "logger.h"
#include "../synch/SyncQueue.h"

namespace CommonLib
{
	namespace file
	{
		class IFile;
	}

	class CLogFileWriterBase : public IlogWriter
	{
	public:

		CLogFileWriterBase(size_t maxLogSize, std::shared_ptr<IlogRetention>& pLogRetention);
		~CLogFileWriterBase();

		virtual void Open(const std::string& path);
		virtual void Close();
		virtual const std::string& GetPath() const;
		virtual const std::string& GetName() const;
		virtual size_t GetSize() const;

	protected:

		void ChangeCurrentLogFile();

	protected:
        std::string m_logName;
        std::string m_path;
		size_t m_maxLogSize;
		size_t m_currSize;
		std::shared_ptr<IlogRetention> m_logRetention;

		std::shared_ptr<file::IFile> m_file;

	};
}

