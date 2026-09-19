#include <iomanip>
#include <sstream>
#include <ctime>
#include "DateUtil.h"

namespace CommonLib
{
	namespace dateutil
	{
        std::uint64_t CDateUtil::GetCurrentDateTime()
		{
			time_t  currTime;
			time(&currTime);

			tm currTm{};
#ifdef _WIN32
			localtime_s(&currTm, &currTime);
#else
			localtime_r(&currTime, &currTm);
#endif

            std::uint64_t date = std::uint64_t(1900 + currTm.tm_year) * 10000000000 + (uint64_t)currTm.tm_mday * 100000000 + (uint64_t)(currTm.tm_mon + 1) * 1000000;
            std::uint64_t time = currTm.tm_hour * 10000 + currTm.tm_min * 100 + currTm.tm_sec;
	 
			return date + time;
		}

        std::string CDateUtil::TimeToString(std::uint64_t time)
        {
            return TimeToString(time, "%Y%m%d%H%M%S", 25);
        }

        std::string CDateUtil::TimeToString(std::uint64_t time, const std::string& format)
        {
            return TimeToString(time, format, 25);
        }

        std::string CDateUtil::TimeToString(std::uint64_t time, size_t maxLength)
        {
            return TimeToString(time, "%Y%m%d%H%M%S", maxLength);
        }

        std::string CDateUtil::TimeToString(std::uint64_t time, const std::string& format, size_t maxLength)
        {
            auto stamp = (time_t)time;
            tm timeInfo{};
#ifdef _WIN32
            localtime_s(&timeInfo, &stamp);
#else
            localtime_r(&stamp, &timeInfo);
#endif
            const size_t bufSize = 50;
            char timeStrbuf[bufSize];

            size_t timeStrLen = strftime(timeStrbuf, bufSize, format.c_str(), &timeInfo);
            return {timeStrbuf, std::min(timeStrLen, maxLength) };
        }

        uint64_t CDateUtil::StringToTime(const std::string& str)
        {
            std::tm tm{};
            std::istringstream ss(str);
            ss >> std::get_time(&tm, "%Y%m%d%H%M%S");

            if (ss.fail())
            {
                std::stringstream errorStream;
                errorStream << "Failed to parse DateString [" << str << "]";
                throw std::runtime_error(errorStream.str());
            }

            std::time_t time = mktime(&tm);
            return time;
        }
	}
}