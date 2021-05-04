#include "stdafx.h"
#include "SignedNumLenEncoder.h"

namespace bptreedb
{
	CSignedNumLenEncoder::CSignedNumLenEncoder(TAllocsSetPtr ptrAllocsSet, TCompressorParamsBasePtr ptrParams, ECompressParams type) :
		TBase(ptrAllocsSet, ptrParams, type)
	{
	 
	}

	CSignedNumLenEncoder::~CSignedNumLenEncoder()
	{

	}

	bool CSignedNumLenEncoder::BeginEncoding(CommonLib::IWriteStream *ptrStream)
	{
		m_pos = 0;
		m_boolEncoder.BeginEncoding(ptrStream);
		return TBase::BeginEncoding(ptrStream);
	}
 
	void CSignedNumLenEncoder::BeginDecoding(CommonLib::IReadStream *ptrStream)
	{
		m_pos = 0;
		m_boolEncoder.BeginDecoding(ptrStream);
		return TBase::BeginDecoding(ptrStream);
	}

	uint32_t  CSignedNumLenEncoder::GetCompressSize() const
	{
		return TBase::GetCompressSize() + m_boolEncoder.GetCompressSize();
	}

	void CSignedNumLenEncoder::Clear()
	{
		m_pos = 0;
		m_boolEncoder.Clear();
		TBase::Clear();
	}
}