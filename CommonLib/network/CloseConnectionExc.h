#pragma once
#include "../exception/exc_base.h"

namespace CommonLib
{
	namespace network
	{
		class CCloseConnection : public CExcBase
		{
		public:
			CCloseConnection()
			{}

			template<typename... Types>
			CCloseConnection(const std::string& format, Types&&... args) : CExcBase(format, std::forward<Types>(args)...)
			{
			}

			virtual ~CCloseConnection() noexcept {}
			virtual std::shared_ptr<CExcBase> Clone() const
			{
				std::shared_ptr<CCloseConnection> ptrExc(new CCloseConnection(*this));
				return ptrExc;
			}
		};

	}
}