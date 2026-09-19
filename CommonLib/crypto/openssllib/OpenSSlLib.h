

#include "../crypto.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			class COpenSSlLIb
			{
			public:
				~COpenSSlLIb();
				static COpenSSlLIb& Instance();

				void LockFunction(int mode, int n, const char * file, int line);

			private:
				static void SSLCallbackLockFunction(int mode, int n, const char * file, int line);
				static unsigned long SSLCallbackThreadId();
				static int SSLCallbackLockCryptoAddLockCallback(int *num, int amount, int type, const char *file, int line);
			private:
				COpenSSlLIb();
				static COpenSSlLIb inst;
				std::map<int, std::shared_ptr<std::recursive_mutex> > m_locksMap;

			};
		}
	}
}