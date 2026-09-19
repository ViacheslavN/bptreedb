#pragma once
#include "log.h"

namespace CommonLib
{

	class CLogFilePerformer : public IlogLineFormat
	{
	public:
		CLogFilePerformer();
		~CLogFilePerformer();

		virtual void FormatLogline(const std::string& msg, eMsgType type, std::string& outMsg);
	private:
		void AddCurrentTime(std::string& msg);
		void AddThreadID(std::string& msg);
		void AddTypeMsg(eMsgType type, std::string& msg);
		void AddMsg(const std::string& msg, std::string& outMsg);

	};
}
