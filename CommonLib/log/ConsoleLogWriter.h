#pragma once

#include "logger.h"

namespace CommonLib
{
	class CConsoleLogWriter : public IlogWriter
	{
	public:
		virtual void Write(const std::string& msg);
	};
}
