#pragma once

#include "../storage/storage.h"

namespace bptreedb
{
	class IPageCipher
	{
	public:
		IPageCipher(){}
		virtual ~IPageCipher(){}

		virtual void encryptPage(IFilePage *pFilePage);
		virtual void decryptPage(IFilePage *pFilePage);
		virtual void encryptPage(IFilePage *pFilePage, byte_t* pDstBuf, uint32_t len);
		virtual void decryptPage(IFilePage *pFilePage, byte_t* pDstBuf, uint32_t len);
		virtual void encryptBuf(byte_t* pBuf, uint32_t len);
		virtual void encryptBuf(const byte_t* pBuf, byte_t* pDstBuf, uint32_t len);
		virtual void decryptBuf(byte_t* pBuf, uint32_t len);

	};

	typedef std::shared_ptr<IPageCipher> IPageCipherPtr;
}