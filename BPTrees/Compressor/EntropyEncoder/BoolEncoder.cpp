#include "stdafx.h"
#include "BoolEncoder.h"
#include "../CompressUtils.h"

namespace bptreedb
{
	CBoolEncoder::CBoolEncoder()
	{
		Clear();
	}

	CBoolEncoder::~CBoolEncoder()
	{

	}

	void CBoolEncoder::Clear()
	{
		m_encodeType = BitEncode;
		m_dataType = ectByte;
		m_bit = false;
		m_bools[0] = 0;
		m_bools[1] = 0;
		m_pos.clear();
		m_bitsRW.Clear();
	}

	void CBoolEncoder::AddSymbol(bool bSign)
	{
		m_bools[bSign ? 1 : 0] += 1;
	}

	void CBoolEncoder::RemoveSymbol(bool bSign)
	{
		m_bools[bSign ? 1 : 0] -= 1;
	}

	uint32_t CBoolEncoder::GetCompressSize() const
	{
		if (m_bools[0] == 0 || m_bools[1] == 0)
			return 1;

		uint32_t nByteSize = ((m_bools[0] + m_bools[1] + 7) / 8) + 1;

		uint32_t nMinCount = min(m_bools[0], m_bools[1]);

		eCompressDataType type = GetCompressType(m_bools[0] + m_bools[1]);
		uint32_t nBytePosCodeSize = GetLenForDiffLen(type, nMinCount + 1);

		return min(nBytePosCodeSize, nByteSize);

	}


	bool CBoolEncoder::BeginEncoding(CommonLib::IWriteStream *pStream)
	{
		byte_t nFlag = 0;
		if (m_bools[0] == 0 || m_bools[1] == 0)
		{
			m_encodeType = OneBit;
			nFlag = m_encodeType;
			if (m_bools[1] != 0)
				nFlag |= (1 << SignBit);

			pStream->Write(nFlag);
		}
		else
		{
 
			CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
			if (!pMemStream)
				throw CommonLib::CExcBase(L"IStream isn't memstream");

			uint32_t nByteSize = ((m_bools[0] + m_bools[1] + 7) / 8) + 1;
			uint32_t nMinCount = min(m_bools[0], m_bools[1]);
			m_dataType = GetCompressType(m_bools[0] + m_bools[1]);
			uint32_t nBytePosCodeSize = GetLenForDiffLen(m_dataType, nMinCount + 1);

			if (nByteSize < nBytePosCodeSize)
			{
				m_encodeType = PosBits;
				pStream->Write((byte_t)m_encodeType);
				m_bitsRW.Attach(pMemStream->Buffer());

				if (!pStream->SeekSafe(nByteSize, CommonLib::soFromCurrent))
					return false;

			}
			else
			{
				m_encodeType = BitEncode;
				pStream->Write((byte_t)m_encodeType);
				if (m_bools[1] != 0)
				{
					nFlag |= (1 << SignBit);
					m_bit = true;
				}

				WriteCompressValue(m_dataType, nMinCount, pStream);

				m_WriteStream.AttachBuffer(pMemStream->Buffer(), nBytePosCodeSize);
				if (!pStream->SeekSafe(nBytePosCodeSize, CommonLib::soFromCurrent))
					return false;
			}
		}

		return true;
	}

	void CBoolEncoder::EncodeBit(bool bit, uint32_t pos)
	{
		switch (m_encodeType)
		{
			case PosBits:
			{
				if (bit == m_bit)
				{
					WriteCompressValue(m_dataType, pos, &m_WriteStream);
				}
				break;
			}
			case BitEncode:
				m_bitsRW.WriteBit(bit);
				break;
		}
	}

	void CBoolEncoder::BeginDecoding(CommonLib::IReadStream *pStream, uint32_t count)
	{
		byte_t nFlag = pStream->ReadByte();
		m_encodeType = (EncodeType)(nFlag & 0x03);

		if (m_encodeType == OneBit)
		{
			m_bit = nFlag & (1 << SignBit) ? true : false;
			m_bools[m_bit ? 1 : 0] = count;
		}
		else if (m_encodeType == PosBits)
		{
			m_pos.resize(count);
			m_bit = nFlag & (1 << 2) ? true : false;
			m_dataType = GetCompressType(count);

			uint32_t posCount = 0;
			ReadCompressValue<uint32_t>(m_dataType, posCount, pStream);
			m_pos.reserve(posCount);
			for (size_t i = 0; i < posCount; ++i)
			{
				uint32_t nPos = 0;
				ReadCompressValue<uint32_t>(m_dataType, nPos, pStream);
				m_pos.push_back(nPos);
			}
		}
		else if (m_encodeType == BitEncode)
		{
			CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
			if (!pMemStream)
				throw CommonLib::CExcBase(L"IStream isn't memstream");

			uint32_t nByteSize = ((count + 7) / 8) + 1;

			m_bitsRW.Attach(pMemStream->Buffer());
			pStream->Seek(nByteSize, CommonLib::soFromCurrent);
		}
	}

	bool CBoolEncoder::DecodeBit(uint32_t pos)
	{
		switch (m_encodeType)
		{
		case OneBit:
			return m_bit;
			break;
		case PosBits:
			if (std::binary_search(m_pos.begin(), m_pos.end(), pos))
				return m_bit;
			return !m_bit;

			break;

		case BitEncode:
			return m_bitsRW.ReadBit();
			break;
		}
	
	}
}