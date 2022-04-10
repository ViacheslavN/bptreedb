#pragma once
#include "UnsignedNumLenEncoder.h"
#include "BoolEncoder.h"


namespace bptreedb
{
	
	class CSignedNumLenEncoder : public CUnsignedNumLenEncoder 
	{
	public:
		CSignedNumLenEncoder(TAllocsSetPtr ptrAllocsSet, TCompressorParamsBasePtr ptrParams, ECompressParams type);
		~CSignedNumLenEncoder();

		typedef CUnsignedNumLenEncoder TBase;


		bool BeginEncoding(CommonLib::IWriteStream *pStream);
		void BeginDecoding(CommonLib::IReadStream *pStream);
		void Clear();

		template <class TSymbol>
		void AddSymbol(const TSymbol& symbol)
		{
			bool bSign = symbol < 0;
			m_boolEncoder.AddSymbol(bSign);
			return TBase::AddSymbol(bSign ? -symbol : symbol);
		}

		template <class TSymbol>
		void RemoveSymbol(const TSymbol& symbol)
		{
			bool bSign = symbol < 0;
			m_boolEncoder.RemoveSymbol(bSign);
			return TBase::RemoveSymbol(bSign ? -symbol : symbol);
		}

		template <class TSymbol>
		bool EncodeSymbol(const TSymbol& symbol)
		{
			bool bSign = symbol < 0;
			m_boolEncoder.EncodeBit(bSign, m_pos++);
			return TBase::EncodeSymbol(bSign ? -symbol : symbol);
		}

		template <class TSymbol>
		void DecodeSymbol(TSymbol& symbol)
		{
			bool bSign = m_boolEncoder.DecodeBit(m_pos++);
			TBase::DecodeSymbol(symbol);
			if (bSign)
				symbol = -symbol;
		}

		uint32_t GetCompressSize() const;

	private:
		CBoolEncoder m_boolEncoder;
		uint32_t m_pos;
	};
}