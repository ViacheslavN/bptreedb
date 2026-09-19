#pragma once

#include "../stream/io.h"
#include "../stream/MemoryStream.h"
#include "../crypto/IDataCipher.h"
#include "BaseSocketIP.h"
#include "../data/ByteArray.h"

namespace CommonLib
{
	namespace network
	{
		class CNetworkStreamIoDev : public io::IWriteRead, io::IWriteReadNotBlocking
		{
		public:
			CNetworkStreamIoDev(CSocketIPPtr pSocket);
			CNetworkStreamIoDev(CSocketIPPtr pSocket, crypto::IDataCipherPtr pDataCipher, IAllocPtr pAlloc);
			
			virtual std::streamsize Write(const byte_t* dataPtr, size_t dataSize);
			virtual std::streamsize Read(byte_t* dataPtr, size_t dataSize);
			virtual std::streamsize WriteNotBlocking(const byte_t* dataPtr, size_t dataSize);
			virtual std::streamsize ReadNotBlocking(byte_t* dataPtr, size_t dataSize);
 
			virtual std::streamsize Read(IMemoryStream *pStream);
			virtual std::streamsize ReadNotBlocking(IMemoryStream *pStream);

		private:
 
			enum eRecvStage
			{
				eReciveHeader,
				eReciveData,
				eDecryptData
			};


			struct SRecvStage 
			{
				eRecvStage m_stage;
				Data::TVecBuffer m_BufForRead;
				uint32_t m_recvSize;
				uint32_t m_received;
				void Reset()
				{
					m_recvSize = 0;
					m_received = 0;
					m_stage = eReciveHeader;
				}
			};

		private:
			std::streamsize RecvData(bool notBlocking);
			bool ReciveHeader(bool notBlocking);
			bool ReciveData(bool notBlocking);
			uint32_t DecryptData( byte_t* dataPtr, size_t dataSize);
			uint32_t DecryptData(IMemoryStream *pStream);
		private:
			CSocketIPPtr m_pSocket;
			crypto::IDataCipherPtr m_pDataCipher;
			Data::TVecBuffer m_BufForCipher;
			SRecvStage m_stage;
			
		};
	}
}