#include "stdafx.h"
#include "FilePage.h"
#include "../utils/CRC.h"

namespace bptreedb
{

	CFilePage::CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr, bool bCheckCRC,
		uint32_t objectID, ObjectPageType objecttype,  uint32_t parentID, ObjectPageType parenttype) : m_pAlloc(pAlloc), m_nSize(nSize), m_nAddr(nAddr), m_bCheckCRC(bCheckCRC),
		m_objectID(objectID), m_objectType(objecttype), m_parentID(parentID), m_parentType(parenttype)
	{

		try
		{
			if (m_pAlloc.get() == nullptr)
				throw CommonLib::CExcBase(L"alloc is null");

			m_pData = (byte_t*)m_pAlloc->Alloc(sizeof(byte_t) * m_nSize);
			byte_t * pPos = GetDataWithoutCRC();

			memcpy(pPos, &m_objectID, sizeof(uint32_t));
			pPos += sizeof(uint32_t);

			memcpy(pPos, &m_objectType, sizeof(uint32_t));
			pPos += sizeof(uint32_t);

			memcpy(pPos, &m_parentID, sizeof(uint32_t));
			pPos += sizeof(uint32_t);

			memcpy(pPos, &m_parentType, sizeof(uint32_t));
 

		}
		catch (CommonLib::CExcBase& exc_src)
		{
			exc_src.AddMsgT(L"CFilePage Create: addr %1, size: %2", nAddr, nSize);
			throw;
		}
	}

	CFilePage::CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, byte_t *pData, uint32_t nSize, int64_t nAddr, bool bCheckCRC): 
		m_pAlloc(pAlloc), m_pData(pData), m_nSize(nSize), m_nAddr(nAddr), m_bCheckCRC(bCheckCRC)
	{
		byte_t * pPos = GetDataWithoutCRC();

		memcpy(&m_objectID, pPos, sizeof(uint32_t));
		pPos += sizeof(uint32_t);

		memcpy(&m_objectType, pPos, sizeof(uint32_t));
		pPos += sizeof(uint32_t);

		memcpy(&m_parentID, pPos, sizeof(uint32_t));
		pPos += sizeof(uint32_t);

		memcpy(&m_parentType, pPos, sizeof(uint32_t));

	}


	byte_t *CFilePage::GetDataWithoutCRC() const
	{
		return m_bCheckCRC ? m_pData + sizeof(uint32_t) : m_pData;
	}

	void CFilePage::ReloadHeader()
	{ 
		//TO DO copy past
		byte_t * pPos = GetDataWithoutCRC();

		memcpy(&m_objectID, pPos, sizeof(uint32_t));
		pPos += sizeof(uint32_t);

		memcpy(&m_objectType, pPos, sizeof(uint32_t));
		pPos += sizeof(uint32_t);

		memcpy(&m_parentID, pPos, sizeof(uint32_t));
		pPos += sizeof(uint32_t);

		memcpy(&m_parentType, pPos, sizeof(uint32_t));
	}

	CFilePage::~CFilePage()
	{
		try
		{
			if (m_pAlloc.get() != nullptr)
				m_pAlloc->Free(m_pData);
		}
		catch (CommonLib::CExcBase& exc)
		{
			exc; // TO DO log;

		}
	}

	int64_t CFilePage::GetPageAddr() const
	{
		return m_nAddr;
	}

	byte_t* CFilePage::GetData()
	{
		return m_bCheckCRC ? m_pData +  sizeof(uint32_t)  + page_header_size : m_pData + page_header_size;
	}

	const byte_t* CFilePage::GetData() const
	{
		return m_bCheckCRC ? m_pData +  sizeof(uint32_t) + page_header_size : m_pData + page_header_size;
	}

	uint32_t CFilePage::GetPageSize() const
	{
		return m_bCheckCRC ? m_nSize - sizeof(uint32_t) - page_header_size : m_nSize - page_header_size;
	}


	byte_t* CFilePage::GetFullData()
	{
		return m_pData;
	}

	const byte_t* CFilePage::GetFullData() const
	{
		return m_pData;
	}

	uint32_t CFilePage::GetFullPageSize() const
	{
		return m_nSize;
	}

	uint32_t CFilePage::GetFlags() const
	{
		return m_nFlags;
	}

	void CFilePage::SetFlag(ePageFlags nFlag, bool bSet)
	{
		if (bSet)
			m_nFlags |= nFlag;
		else
			m_nFlags &= ~nFlag;
	}

	void CFilePage::SetAddr(int64_t nAddr)
	{
		m_nAddr = nAddr;
	}

	bool CFilePage::IsNeedEncrypt() const
	{
		return m_bIsNeedEncrypt;
	}

	void CFilePage::SetNeedEncrypt(bool bEncrypt)
	{
		m_bIsNeedEncrypt = bEncrypt;
	}

	bool CFilePage::CheckCRC() const
	{
		if (!m_bCheckCRC)
			return true; //TO DO probably the best way to throw an exception

		/*uint32_t crc = 0;
		memcpy(&crc, m_pData, sizeof(uint32_t));
		uint32_t calcCrc = utils::Crc32(GetData(), GetPageSize());
		
		return crc == calcCrc;*/

		return CheckCRC(GetFullData(), GetFullPageSize());
	}

	void CFilePage::WriteCRC()
	{
		if (!m_bCheckCRC)
			throw CommonLib::CExcBase(L"Filed to write crc, page dosn't support crc");

		//uint32_t calcCrc = utils::Crc32(GetData(), GetPageSize());
		//memcpy(m_pData, &calcCrc, sizeof(uint32_t));	

		WriteCRC(GetFullData(), GetFullPageSize());
	}

	void CFilePage::WriteCRC(byte_t* pData, uint32_t nSize)
	{
		uint32_t calcCrc = (uint32_t)utils::Crc32(pData + sizeof(uint32_t), nSize - sizeof(uint32_t));
		memcpy(pData, &calcCrc, sizeof(uint32_t));
	}

	bool CFilePage::CheckCRC(const byte_t* pData, uint32_t nSize)
	{
		uint32_t crc = 0;
		memcpy(&crc, pData, sizeof(uint32_t));
		uint32_t calcCrc = (uint32_t)utils::Crc32(pData + sizeof(uint32_t), nSize - sizeof(uint32_t));

		return crc == calcCrc;
	}

	void CFilePage::Erase(byte_t val)
	{ 
		memset(GetData(), val, GetPageSize());
 	}

	ObjectPageType CFilePage::GetObjectType() const
	{
		return (ObjectPageType)m_objectType;
	}

	uint32_t CFilePage::GetObjectID() const
	{
		return m_objectID;
	}

	ObjectPageType CFilePage::GetParentType() const
	{
		return (ObjectPageType)m_parentType;
	}

	uint32_t CFilePage::GetParentObjectID() const
	{
		return m_parentID;
	}

	byte_t* CFilePage::GetMetaData()
	{
		return m_pData + object_id_size;
	}

	const byte_t* CFilePage::GetMetaData() const
	{
		return m_pData + object_id_size;
	}

	uint32_t CFilePage::GetMetaDataSize() const
	{
		return m_metainfo_block_size;
	}

}