#include "stdafx.h"
#include "NetworkStreamIoDev.h"
#include "portable_endian.h"

namespace CommonLib
{
	namespace network
	{
		CNetworkStreamIoDev::CNetworkStreamIoDev(CSocketIPPtr pSocket) : m_pSocket(pSocket)
		{

		}

		CNetworkStreamIoDev::CNetworkStreamIoDev(CSocketIPPtr pSocket, crypto::IDataCipherPtr pDataCipher, IAllocPtr pAlloc) : m_pSocket(pSocket), m_pDataCipher(pDataCipher), m_BufForCipher(Data::TAlloc(pAlloc))
		{

		}

		std::streamsize CNetworkStreamIoDev::Write(const byte_t* dataPtr, size_t dataSize)
		{
			if (m_pDataCipher.get() == nullptr)
			{
				m_pSocket->Send(dataPtr, dataSize);
				return dataSize;
			}

			uint32_t encriptedSize = m_pDataCipher->GetBufferSize((uint32_t)dataSize) ;
			uint32_t fullSize = encriptedSize + sizeof(uint32_t);
			if (m_BufForCipher.size() < (fullSize))
				m_BufForCipher.resize(fullSize);

			*reinterpret_cast<int32_t*>(m_BufForCipher.data()) = htole32(encriptedSize);
			m_pDataCipher->Encrypt(dataPtr, encriptedSize, m_BufForCipher.data() + sizeof(uint32_t), encriptedSize);
			m_pSocket->Send(m_BufForCipher.data(), fullSize);

			return fullSize;
		}

		std::streamsize CNetworkStreamIoDev::Read(byte_t* dataPtr, size_t dataSize)
		{
			RecvData(false);

			if (m_stage.m_stage != eDecryptData || m_stage.m_recvSize != m_stage.m_received)
				return 0;

			return DecryptData(dataPtr, dataSize);

		}

		std::streamsize CNetworkStreamIoDev::WriteNotBlocking(const byte_t* dataPtr, size_t dataSize)
		{
			if (m_pDataCipher.get() == nullptr)
			{
				size_t sendData;
				if (!m_pSocket->SendNotBlocking(dataPtr, dataSize, sendData))
					return 0;

				return sendData;
			}

			uint32_t encriptedSize = m_pDataCipher->GetBufferSize((uint32_t)dataSize);
			uint32_t fullSize = encriptedSize + sizeof(uint32_t);
			if (m_BufForCipher.size() < (fullSize))
				m_BufForCipher.resize(fullSize);

			*reinterpret_cast<int32_t*>(m_BufForCipher.data()) = htole32(encriptedSize);
			m_pDataCipher->Encrypt(dataPtr, encriptedSize, m_BufForCipher.data() + sizeof(uint32_t), encriptedSize);

			size_t sendData;
			if (!m_pSocket->SendNotBlocking(m_BufForCipher.data(), fullSize, sendData))
				return 0;


			return sendData;
		}

		std::streamsize CNetworkStreamIoDev::ReadNotBlocking(byte_t* dataPtr, size_t dataSize)
		{

			RecvData(true);

			if (m_stage.m_stage != eDecryptData || m_stage.m_recvSize != m_stage.m_received)
				return 0;

			return DecryptData(dataPtr, dataSize);

		}

		std::streamsize CNetworkStreamIoDev::Read(IMemoryStream *pStream)
		{
			RecvData(false);
			if (m_stage.m_stage != eDecryptData || m_stage.m_recvSize != m_stage.m_received)
				return 0;

			return DecryptData(pStream);
		}

		std::streamsize CNetworkStreamIoDev::ReadNotBlocking(IMemoryStream *pStream)
		{
			RecvData(true);
			if (m_stage.m_stage != eDecryptData || m_stage.m_recvSize != m_stage.m_received)
				return 0;

			return DecryptData(pStream);
		}

		std::streamsize CNetworkStreamIoDev::RecvData( bool notBlocking)
		{
			bool bProccess = true;
			std::streamsize recvSize = 0;
			m_stage.Reset();

			while (bProccess)
			{
				switch (m_stage.m_stage)
				{
					case eReciveHeader:
						bProccess = ReciveHeader(notBlocking);
						break;
					case eReciveData:
						bProccess = ReciveData(notBlocking);
						break;
				}
			}

			return m_stage.m_received;
		}

		bool CNetworkStreamIoDev::ReciveHeader(bool notBlocking)
		{


			byte_t* pBuffer = (byte_t*)&m_stage.m_recvSize + m_stage.m_received;
			uint32_t sizeToRead = sizeof(uint32_t) - m_stage.m_received;

			if (notBlocking)
			{
				size_t recvData = 0;
				if (!m_pSocket->RecvNotBlocking(pBuffer, sizeToRead, recvData))
					return false;
 
				m_stage.m_received += (uint32_t)recvData;
			}
			else
			{
				m_pSocket->Recv(pBuffer, sizeToRead);
				m_stage.m_received += sizeToRead;
			}

			if (sizeof(uint32_t) == m_stage.m_received)
			{
				m_stage.m_stage = eReciveData;
				m_stage.m_received = 0;
				m_stage.m_recvSize = le32toh(m_stage.m_recvSize);
				m_stage.m_BufForRead.resize(m_stage.m_recvSize);
			}

			return true;
		}

		bool CNetworkStreamIoDev::ReciveData(bool notBlocking)
		{
			byte_t* pBuffer = m_stage.m_BufForRead.data() + m_stage.m_received;
			uint32_t sizeToRead = m_stage.m_recvSize - m_stage.m_received;
			if (notBlocking)
			{
				size_t recvData = 0;
				if (!m_pSocket->RecvNotBlocking(pBuffer, sizeToRead, recvData))
					return false;

				m_stage.m_received += (uint32_t)recvData;
			}
			else
			{
				m_pSocket->Recv(pBuffer, sizeToRead);
				m_stage.m_received += sizeToRead;
			}

			if (m_stage.m_recvSize == m_stage.m_received)
			{
				m_stage.m_stage = eDecryptData;
			}

			return true;
		}

		uint32_t CNetworkStreamIoDev::DecryptData(byte_t* dataPtr, size_t dataSize)
		{
			m_BufForCipher.resize(m_stage.m_recvSize);
			uint32_t decSize = m_pDataCipher->Decrypt(m_stage.m_BufForRead.data(), m_stage.m_recvSize, m_BufForCipher.data(), (uint32_t)m_BufForCipher.size());
			if (dataSize != decSize)
				throw CommonLib::CExcBase("Wrong read size {0}, dec size {1}", dataSize, decSize);

			memcpy(dataPtr, m_BufForCipher.data(), decSize);
			return decSize;
		}

		uint32_t CNetworkStreamIoDev::DecryptData(IMemoryStream *pStream)
		{

			pStream->Resize(m_stage.m_recvSize);
			uint32_t decSize = m_pDataCipher->Decrypt(m_stage.m_BufForRead.data(), m_stage.m_recvSize, m_BufForCipher.data(), (uint32_t)m_BufForCipher.size());
			pStream->Resize(decSize);

			return decSize;
		}
 
	}
}