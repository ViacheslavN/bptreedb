#pragma once
#include "logger.h"
#include "../str/str.h"

namespace CommonLib
{
	class CLogInfo
	{
	public:
		template<typename... Types>
		CLogInfo(CLogger log, const astr& format, Types&&... args): m_log(log), m_complete(false)
		{
			m_msg = str_format::AStrFormatSafeT(format, std::forward<Types>(args)...);
			m_log.Info(m_msg);
		}

		CLogInfo(CLogger log, const std::string& msg) : m_log(log), m_complete(false)
		{
			m_msg = msg;
			m_log.Info(m_msg); 
		}

		void Complete()
		{
			Complete(m_msg + "  ok");
		}

		void Complete(const std::string& msg)
		{
			m_complete = true;
			m_log.Info(msg);
		}

		template<typename... Types>
		void Complete(const std::string& format, Types&&... args)
		{
			std::string msg = str_format::AStrFormatSafeT(format, std::forward<Types>(args)...);
			Complete(msg);
		}


		~CLogInfo()
		{
			if (m_complete)
				return;

			m_log.Error(m_msg);
		}


	private:
		CLogger& m_log;
		bool m_complete;
		std::string m_msg;
	};


	class CLogTrace
	{
	public:
		template<typename... Types>
		CLogTrace(CLogger log, const astr& format, Types&&... args) : m_log(log), m_complete(false)
		{
			m_msg = str_format::AStrFormatSafeT(format, std::forward<Types>(args)...);
			m_log.Trace(m_msg);
		}


		CLogTrace(CLogger log, const astr& msg) : m_log(log), m_complete(false)
		{
			m_msg = msg;
			m_log.Trace(m_msg);
		}

		void Complete()
		{
			m_log.Trace(m_msg + "  ok");
		}

		void Complete(const astr& msg)
		{
			m_complete = true;
			m_log.Trace(msg);
		}

		template<typename... Types>
		void Complete(const astr& format, Types&&... args)
		{
			astr msg = str_format::AStrFormatSafeT(format, std::forward<Types>(args)...);
			m_log.Trace(msg);
		}


		~CLogTrace()
		{
			if (m_complete)
				return;

			m_log.Trace(m_msg + "  Error");
		}


	private:
		CLogger& m_log;
		bool m_complete;
		astr m_msg;
	};

}