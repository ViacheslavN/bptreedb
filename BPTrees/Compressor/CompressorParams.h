
#pragma once

#include "../../bptreedb.h"
#include "../../../CommonLib/stream/MemoryStream.h"
#include "../../../CommonLib/stream/FixedMemoryStream.h"

namespace bptreedb
{

	enum eCompressorParamsID
	{
		eBaseCompID = 1,
		eComposeIndexCompID = 2
	};

	class ICompressorParams
	{
	public:
		virtual ~ICompressorParams() {}
		virtual eCompressorParamsID GetCompressorParmasID() const = 0;
	};

	template <class _Transaction>
	class CompressorParamsBase : public ICompressorParams
	{
	public:
		CompressorParamsBase()
		{}
		virtual ~CompressorParamsBase() {}

		virtual void Load(_Transaction *pTran) = 0;
		virtual void Save(_Transaction *pTran) = 0;
	};
	

	class CompressorParamsBaseImp
	{
	public:
		CompressorParamsBaseImp() : m_compressType(ACCoding), m_bCalcOnlineSize(false), m_nErrorCalcMiliproñ(100), m_nMaxRowCoeff(1)
		{}
		virtual ~CompressorParamsBaseImp() {}



		virtual void Load(CommonLib::IReadStream *pStream)
		{
			m_compressType = (EncoderType)pStream->Readint16();
			m_bCalcOnlineSize = pStream->ReadBool();
			m_nErrorCalcMiliproñ = pStream->ReadIntu32();
			m_nMaxRowCoeff = pStream->ReadIntu32();
		}

		virtual void Save(CommonLib::IWriteStream *pStream)
		{
			pStream->write(uint16_t(m_compressType));
			pStream->write(m_bCalcOnlineSize);
			pStream->write(m_nErrorCalcMiliproñ);
			pStream->write(m_nMaxRowCoeff);
		 
		}

		template<class _Transaction>
		void SaveState(_Transaction * pTransaction)
		{

		}

		EncoderType m_compressType;
		bool m_bCalcOnlineSize;
		uint32_t m_nErrorCalcMiliproñ;
		uint32_t m_nMaxRowCoeff;
	};
}

 