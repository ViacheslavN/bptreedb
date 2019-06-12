#pragma once
#include "storage.h"
#include "../../commonlib/alloc/alloc.h"

namespace bptreedb
{

class CFilePage : public IFilePage
{
public:

	CFilePage(std::shared_ptr<CommonLib::IAlloc> pAlloc, uint32_t nSize, int64_t nAddr, bool bCheckCRC);
	CFilePage(byte_t *pData, uint32_t nSize, int64_t nAddr, bool bCheckCRC);

	virtual ~CFilePage();

	virtual int64_t GetPageAddr() const;
	virtual byte_t* GetData();
	virtual const byte_t* GetData() const;
	virtual uint32_t GetPageSize() const;
	virtual byte_t* GetFullData();
	virtual const byte_t* GetFullData() const;
	virtual uint32_t GetFullPageSize() const;
	virtual uint32_t GetFlags() const;
	virtual void SetFlag(ePageFlags nFlag, bool bSet);
	virtual bool IsNeedEncrypt() const;
	virtual void SetNeedEncrypt(bool bEncrypt);
	virtual bool CheckCRC() const ;
	virtual void WriteCRC();

	static void WriteCRC(byte_t* pData, size_t nSize);
	static bool CheckCRC(const byte_t* pData, size_t nSize);

private:
	uint32_t m_nFlags{ 0 };
	int64_t m_nAddr{ -1 };
	byte_t *m_pData{ nullptr };
	uint32_t m_nSize{ 0 };
	bool m_bCheckCRC{ false };
	std::shared_ptr<CommonLib::IAlloc> m_pAlloc;
	bool m_bIsNeedEncrypt{ false };
};
 

}