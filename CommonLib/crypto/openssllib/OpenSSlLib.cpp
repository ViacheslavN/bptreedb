#include "stdafx.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include "OpenSSlLib.h"
#include "synch/thread.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			COpenSSlLIb COpenSSlLIb::inst;

			COpenSSlLIb::COpenSSlLIb()
			{
				OpenSSL_add_all_algorithms();
				ERR_load_crypto_strings();
				RAND_status();

				if (CRYPTO_get_locking_callback() != nullptr)
				{
					CRYPTO_set_locking_callback(COpenSSlLIb::SSLCallbackLockFunction);
				}

				CRYPTO_set_id_callback(COpenSSlLIb::SSLCallbackThreadId);
			}

			COpenSSlLIb::~COpenSSlLIb()
			{
				EVP_cleanup();
				ERR_free_strings();
			}

			COpenSSlLIb& COpenSSlLIb::Instance()
			{
				return inst;
			}

			unsigned long COpenSSlLIb::SSLCallbackThreadId()
			{
				return synch::CThread::GetCurThreadId();
			}

#ifdef _WIN32
			int COpenSSlLIb::SSLCallbackLockCryptoAddLockCallback(int *num, int amount, int type, const char *file, int line)
			{
				return	::InterlockedExchangeAdd((long*)num, amount);
			}
#endif

			void  COpenSSlLIb::SSLCallbackLockFunction(int mode, int n, const char * file, int line)
			{
				COpenSSlLIb::Instance().LockFunction(mode, n, file, line);
			}

			void  COpenSSlLIb::LockFunction(int mode, int n, const char * file, int line)
			{
				auto it = m_locksMap.find(n);
				if (it == m_locksMap.end())
					it = m_locksMap.insert(std::make_pair(n, std::shared_ptr<std::recursive_mutex>(new std::recursive_mutex()))).first;

				if (mode & CRYPTO_LOCK)
					it->second->lock();
				else
					it->second->unlock();
			}


		}
	}
}