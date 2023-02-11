#pragma once

namespace bptreedb
{
	typedef std::shared_ptr<class IDatabaseLog> IDatabaseLogPtr;

	enum LogMessageType
	{
		INFO = 1,
		WARNING,
		ERROR
	};

	class IDatabaseLog
	{
	public:
		IDatabaseLog() {};
		virtual ~IDatabaseLog() {};

		virtual void AddMessage(LogMessageType type, const astr& message) = 0;
	};

}