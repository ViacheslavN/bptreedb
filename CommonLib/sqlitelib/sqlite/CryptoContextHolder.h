#pragma once

#include "../Database.h"

namespace CommonLib
{
	namespace database
	{

		class CCryptoContextHolder
		{
		public:
			static CCryptoContextHolder& Instance();

			ICryptoContextPtr GetCryptoContext(const std::string& databaseName) const;
			void AddCryptoContext(const std::string& databaseName, ICryptoContextPtr ptrContext);
			void RemoveCryptoContext(const std::string& databaseName);

			~CCryptoContextHolder();

		private:
			CCryptoContextHolder();
			typedef std::map<std::string, ICryptoContextPtr> TMapCryptoContexts;
			TMapCryptoContexts m_cryptoContexts;
		};



	}
}
