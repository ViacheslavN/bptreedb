#pragma once

namespace CommonLib
{
	namespace dateutil
	{
		class CDateUtil
		{
		public:
			static std::uint64_t GetCurrentDateTime(); //YYYYDDMMHHMMSS
            static std::string TimeToString(std::uint64_t time); //YYYYDDMMHHMMSS
            static std::string TimeToString(std::uint64_t time, const std::string& format);
            static std::string TimeToString(std::uint64_t time, size_t maxLength); //YYYYDDMMHHMMSS, for time partitioning
            static std::string TimeToString(std::uint64_t time, const std::string& format, size_t maxLength);
            static std::uint64_t StringToTime(const std::string& str); //YYYYDDMMHHMMSS
		};
	}
}