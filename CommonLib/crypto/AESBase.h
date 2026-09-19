#pragma once
#include <vector>
#include "crypto.h"
 

namespace CommonLib
{
	namespace crypto
	{
		class CAESBase : public IAESCipher
		{
		public:
			CAESBase(EAESKeySize keySize, bool bPadding, CipherChainMode mode);
			virtual ~CAESBase();

			virtual uint32_t GetKeySize() const;
			virtual uint32_t GetIVSize() const;
			virtual byte_t* GetIVData();
			virtual const byte_t* GetIVData() const;

			virtual uint32_t GetBufferSize(uint32_t bufDataSize) const;
			virtual uint32_t GetBlockSize() const;

			void EncryptThrowIfWrongtSize(uint32_t plainInSize, uint32_t dstBufSize) const;

		protected:
			bool m_bPadding;
			uint32_t m_nBlockSize;
			EAESKeySize m_keySize;
			CipherChainMode m_chainMode;
			crypto_vector m_IvData;
		};
	}
}