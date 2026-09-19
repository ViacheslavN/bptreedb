#include "stdafx.h"
#include "XTSBlockChiper.h"
#include "exception/exc_base.h"

namespace CommonLib
{
	namespace crypto
	{
 
		CXTSBlockChiper::CXTSBlockChiper(IDataCipherPtr ptrDataCipher, IDataCipherPtr ptrTweakCipher) : m_ptrDataCipher(ptrDataCipher),m_ptrTweakCipher(ptrTweakCipher)
		{
			if (m_ptrDataCipher.get() == nullptr)
				throw CExcBase("XTSBlockChiper: DataCipher is null");

			if (m_ptrTweakCipher.get() == nullptr)
				throw CExcBase("XTSBlockChiper: TweakCipher is null");

			m_tweak.resize(CIPHER_BLOCK_SIZE);
			m_finishTweak.resize(CIPHER_BLOCK_SIZE);
		}
		
		CXTSBlockChiper::~CXTSBlockChiper()
		{
		}

		uint32_t CXTSBlockChiper::Encrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t srcSize, byte_t* dstBuf, uint32_t dstSize)
		{
			if (srcSize < dstSize)
				throw CExcBase("XTSBlockChiper: wrong dst size, src: {0}, dst: {1}", srcSize, dstSize);

			memcpy(dstBuf, srcBuf, srcSize);
			return Encrypt(nBlockId, dstBuf, srcSize);
		}

		uint32_t CXTSBlockChiper::Decrypt(int64_t nBlockId, const byte_t* srcBuf, uint32_t bufSize, byte_t* dstBuf, uint32_t dstSize)
		{
			return 0;
		}

		uint32_t CXTSBlockChiper::Encrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t srcSize)
		{
			try
			{
				InitTweak(nBlockId);
				uint32_t i = 0;
				for (; i + CIPHER_BLOCK_SIZE <= srcSize; i += CIPHER_BLOCK_SIZE)
				{
					if (i > 0)
						UpdateTweak();

					XorTweak(srcBuf + i, m_tweak);
					m_ptrDataCipher->Encrypt(srcBuf + i, CIPHER_BLOCK_SIZE);
					XorTweak(srcBuf + i, m_tweak);
				}

				if (i < srcSize)
				{
					UpdateTweak();
					Swap(srcBuf, i , i - CIPHER_BLOCK_SIZE, srcSize - i);
					XorTweak(srcBuf + i - CIPHER_BLOCK_SIZE, m_tweak);

					m_ptrDataCipher->Encrypt(srcBuf + i - CIPHER_BLOCK_SIZE, CIPHER_BLOCK_SIZE);

					XorTweak(srcBuf + i - CIPHER_BLOCK_SIZE, m_tweak);
				}

				return srcSize;
			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExcT("XTSBlockChiper: Failed to encrypt, block id {0}, src: {1}", nBlockId, srcSize, exc);
				throw;
			}
		}

		uint32_t CXTSBlockChiper::Decrypt(int64_t nBlockId, byte_t* srcBuf, uint32_t srcSize)
		{
			try
			{
				bool tail = (srcSize % CIPHER_BLOCK_SIZE) == 0 ? false : true;
				
				InitTweak(nBlockId);
				if (tail)
					m_finishTweak = m_tweak;

				uint32_t i = 0;
				for (; i + CIPHER_BLOCK_SIZE <= srcSize; i += CIPHER_BLOCK_SIZE) {
					if (i > 0)
					{
						UpdateTweak();
						if (i + CIPHER_BLOCK_SIZE + CIPHER_BLOCK_SIZE > srcSize && 	i + CIPHER_BLOCK_SIZE < srcSize)
						{
							m_finishTweak = m_tweak;
							UpdateTweak();
						}
					}
					XorTweak(srcBuf + i, m_tweak);
					m_ptrDataCipher->Decrypt(srcBuf + i, CIPHER_BLOCK_SIZE);
					XorTweak(srcBuf + i, m_tweak);
				}

				if (i < srcSize)
				{
					Swap(srcBuf, i, i - CIPHER_BLOCK_SIZE, srcSize - i);
					XorTweak(srcBuf + i - CIPHER_BLOCK_SIZE, m_finishTweak);
					m_ptrDataCipher->Decrypt(srcBuf + i - CIPHER_BLOCK_SIZE, CIPHER_BLOCK_SIZE);
					XorTweak(srcBuf + i - CIPHER_BLOCK_SIZE, m_finishTweak);
				}				

				return srcSize;
			}
			catch (std::exception& exc)
			{
				CExcBase::RegenExcT("XTSBlockChiper: Failed to decrypt, block id {0}, src: {1}", nBlockId, srcSize, exc);
				throw;
			}

		}

		void CXTSBlockChiper::InitTweak(uint64_t nBlockId)
		{
			for (uint32_t i = 0; i < CIPHER_BLOCK_SIZE; i++, nBlockId >>= 8)
			{
				m_tweak[i] = (byte_t)(nBlockId & 0xFF);
			}

			m_ptrTweakCipher->Encrypt(m_tweak.data(), (uint32_t)m_tweak.size());
		}

		void CXTSBlockChiper::UpdateTweak()
		{
			byte_t ci = 0, co = 0;
			for (uint32_t  i = 0; i < CIPHER_BLOCK_SIZE; i++)
			{
				co = (byte_t)((m_tweak[i] >> 7) & 1);
				m_tweak[i] = (byte_t)(((m_tweak[i] << 1) + ci) & 0xff);
				ci = co;
			}

			if (co != 0)
				m_tweak[0] ^= GF_128_FDBAK;
		}

		void CXTSBlockChiper::XorTweak(byte_t *pData, const std::vector<byte_t>& tweak)
		{
			for (uint32_t i = 0; i < CIPHER_BLOCK_SIZE; i++) 
				pData[i] ^= tweak[i];
	
		}

		void  CXTSBlockChiper::Swap(byte_t *pData, uint32_t source, uint32_t target, uint32_t len)
		{
			for (uint32_t i = 0; i < len; i++)
			{
				byte_t temp = pData[source + i];
				pData[source + i] = pData[target + i];
				pData[target + i] = temp;
			}
		}
	}
}