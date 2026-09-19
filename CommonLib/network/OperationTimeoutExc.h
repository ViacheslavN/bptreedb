#pragma once
#include "../exception/exc_base.h"

namespace CommonLib
{
	namespace network
	{
		class COperationTimeout : public CExcBase
		{
		public:
			COperationTimeout(long timeout) : m_timeout(timeout)
			{}

			template<typename... Types>
			COperationTimeout(long timeout, const std::string& format, Types&&... args) : CExcBase(format, std::forward<Types>(args)...), m_timeout(timeout)
			{
			}

			virtual ~COperationTimeout() noexcept {}
			virtual std::shared_ptr<CExcBase> Clone() const
			{
				std::shared_ptr<COperationTimeout> ptrExc(new COperationTimeout(*this));
				return ptrExc;
			}

			long GetTimeout() const { return m_timeout; }
		private:
			long m_timeout;
		};
	}
}