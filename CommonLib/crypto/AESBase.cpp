#include "stdafx.h"
#include "AESBase.h"
#include "exception/exc_base.h"

namespace CommonLib
{
	namespace crypto
	{

		#define AES_BLOCK_SIZE 16

		CAESBase::CAESBase(EAESKeySize keySize, bool bPadding, CipherChainMode mode) :
			m_keySize(keySize), m_bPadding(bPadding), m_chainMode(mode) , m_nBlockSize(AES_BLOCK_SIZE)
		{
			m_IvData.resize(m_nBlockSize, 0);
		}

		CAESBase::~CAESBase()
		{

		}

		uint32_t CAESBase::GetKeySize() const
		{
			switch (m_keySize)
			{
			case CommonLib::crypto::AES_128:
				return 16;
			case CommonLib::crypto::AES_192:
				return 24;
			case CommonLib::crypto::AES_256:
				return 32;
			default:
				throw CExcBase("AESCipher: unknown type {0}", m_keySize);
				break;
			}
		}

		uint32_t CAESBase::GetBlockSize() const
		{
			return m_nBlockSize;
		}

		uint32_t CAESBase::GetBufferSize(uint32_t bufDataSize) const
		{
			if (m_bPadding)
				return (bufDataSize / GetBlockSize() + 1) * GetBlockSize();

			return (bufDataSize / GetBlockSize()) * GetBlockSize();
		}

		uint32_t CAESBase::GetIVSize() const
		{
			return (uint32_t)m_IvData.size();
		}

		byte_t* CAESBase::GetIVData()
		{
			return m_IvData.data();
		}

		const byte_t* CAESBase::GetIVData() const
		{
			return m_IvData.data();
		}

		void CAESBase::EncryptThrowIfWrongtSize(uint32_t plainInSize, uint32_t dstBufSize) const
		{
			if (!m_bPadding)
			{

				if (plainInSize % GetBlockSize())
					throw CExcBase("Wrong plain size {0}, without padding size must be multiple {1}", plainInSize, GetBlockSize());
			}

			if(dstBufSize < GetBufferSize(plainInSize))
				throw CExcBase("Wrong output size {0}, expected {1}", dstBufSize, GetBufferSize(plainInSize));

		}
		
	}
}