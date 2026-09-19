#pragma once

namespace CommonLib
{
	enum eMsgType
	{
		eTrace = 1,
		eInfo = 2,
		eWarning = 4,
		eError = 8
	};

	class IlogLineFormat
	{
	public:
		IlogLineFormat() {}
		virtual ~IlogLineFormat(){}
		virtual void FormatLogline(const std::string& msg, eMsgType type, std::string& outMsg) = 0;
	};	

	class IlogRetention
	{
	public:
		IlogRetention() {}
		virtual ~IlogRetention() {}

		virtual void LogProcessing(const std::string& logDirPath, const std::string& logFileName) = 0;
		virtual void ArcLogFile(const std::string& logPath) = 0;
	};

	class IlogWriter
	{
	public:
		IlogWriter() {}
		virtual ~IlogWriter() {}

		IlogWriter(const IlogWriter&) = delete;
		IlogWriter& operator=(const IlogWriter&) = delete;

	//	virtual void Open(const std::string& path) = 0;
	//	virtual void Close() = 0;
		virtual void Write(const std::string& msg) = 0;
		//virtual const std::string& GetPath() const = 0;
		//virtual const std::string& GetName() const = 0;
		//virtual size_t GetSize() const = 0;
	};
}