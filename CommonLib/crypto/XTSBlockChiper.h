#pragma once
#include "crypto.h"

namespace CommonLib
{
	namespace crypto
	{
		typedef std::shared_ptr<class CXTSBlockChiper> CXTSBlockChiperPtr;

		class CXTSBlockChiper : public IXTSDataCipher
		{
			const uint32_t GF_128_FDBAK = 0x87;
			const uint32_t CIPHER_BLOCK_SIZE = 16;
		public:
			CXTSBlockChiper(IDataCipherPtr ptrDataCipher, IDataCipherPtr ptrTweakCipher);
			virtual ~CXTSBlockChiper();

			virtual uint32_t Encrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
			virtual uint32_t Decrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize);
			virtual uint32_t Encrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t bufSize);
			virtual uint32_t Decrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t bufSize);
		private:
			void InitTweak(uint64_t nBlockId);
			void UpdateTweak();
			void XorTweak(byte_t *pData, const std::vector<byte_t>& tweak);
			void Swap(byte_t *pData, uint32_t source, uint32_t target, uint32_t len);
		private:
			std::vector<byte_t> m_tweak;
			std::vector<byte_t> m_finishTweak;

			IDataCipherPtr m_ptrDataCipher;
			IDataCipherPtr m_ptrTweakCipher;
		};

	}
}