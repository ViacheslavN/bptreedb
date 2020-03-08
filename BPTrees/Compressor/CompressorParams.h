
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


	class CompressorParamsBase
	{
	public:
		CompressorParamsBase() : m_compressType(ACCoding), m_bCalcOnlineSize(false), m_nErrorCalcMiliproñ(100), m_nMaxRowCoeff(1)
		{}
		virtual ~CompressorParamsBase() {}



		virtual void Load(CommonLib::IReadStream *pStream)
		{
			m_compressType = (EncoderType)pStream->Readint16();
			m_bCalcOnlineSize = pStream->ReadBool();
			m_nErrorCalcMiliproñ = pStream->ReadIntu32();
			m_nMaxRowCoeff = pStream->ReadIntu32();
		}

		virtual void Save(CommonLib::IWriteStream *pStream)
		{
			pStream->Write(uint16_t(m_compressType));
			pStream->Write(m_bCalcOnlineSize);
			pStream->Write(m_nErrorCalcMiliproñ);
			pStream->Write(m_nMaxRowCoeff);
		 
		}

		EncoderType m_compressType;
		bool m_bCalcOnlineSize;
		uint32_t m_nErrorCalcMiliproñ;
		uint32_t m_nMaxRowCoeff;
	};

	typedef std::shared_ptr<CompressorParamsBase> TCompressorParamsBasePtr;
}

 