#pragma once

#include "../storage/storage.h"

namespace bptreedb
{
	class IPageCipher
	{
	public:
		IPageCipher(){}
		virtual ~IPageCipher(){}

		virtual void EncryptPage(IFilePage *pFilePage);
		virtual void DecryptPage(IFilePage *pFilePage);
		virtual void EncryptPage(IFilePage *pFilePage, byte_t* pDstBuf, uint32_t len);
		virtual void DecryptPage(IFilePage *pFilePage, byte_t* pDstBuf, uint32_t len);
		virtual void EncryptBuf(byte_t* pBuf, uint32_t len);
		virtual void EncryptBuf(const byte_t* pBuf, byte_t* pDstBuf, uint32_t len);
		virtual void DecryptBuf(byte_t* pBuf, uint32_t len);

	};

	typedef std::shared_ptr<IPageCipher> IPageCipherPtr;
}