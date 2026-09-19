#include "../crypto.h"

namespace CommonLib
{
	namespace crypto
	{
		namespace openssllib
		{
			class CRandomGeneratorSSL : public IRandomGenerator
			{
			public:
				CRandomGeneratorSSL();
				virtual ~CRandomGeneratorSSL();

				virtual void GenRandom(crypto_vector& random);
				virtual void GenRandom(byte_t* pData, uint32_t size);
			private:
 
			};
		}
	}
}