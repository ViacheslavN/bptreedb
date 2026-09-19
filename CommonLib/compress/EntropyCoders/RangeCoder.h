#pragma once
#include "../../stream/MemoryStream.h"
#include "BaseACCoder.h"
namespace CommonLib
{
	template<class _TCodeValue, uint16_t _nValueBits>
	class TRangeEncoder : public BaseACEncoder<_TCodeValue>
	{

		typedef _TCodeValue TCodeValue;
		static const uint16_t nValueBits = _nValueBits - 8;
		static const _TCodeValue Top = (_TCodeValue)1 << nValueBits;
		static const _TCodeValue Bottom = (_TCodeValue)1 << (nValueBits - 8);
		_TCodeValue Low, Range;
	public:

		static const _TCodeValue MaxRange = Bottom;
		TRangeEncoder() :
			m_pStream(nullptr), Low(0), Range((_TCodeValue)-1) 
		{
			//assert(m_pStream);
		}

		void SetStream(IWriteStream* pStream)
		{
			m_pStream = pStream;
			Low = 0;
			Range = (_TCodeValue)-1;
		}

		virtual bool EncodeSymbol(_TCodeValue SymbolLow, _TCodeValue SymbolHigh, _TCodeValue TotalRange)
		{

			Low += SymbolLow * (Range /= TotalRange);
			Range *= SymbolHigh - SymbolLow;

			while ((Low ^ (Low+Range))<Top ||	Range < Bottom &&	((Range= -Low & (Bottom-1)),1))
				{
				
				if (!m_pStream->WriteSafe(byte_t((Low >> nValueBits) & 0xFF)))
					return false;

					Range <<= 8;
					Low <<= 8;
				}
	/*		for (;;)
			{
				if ((Low ^ (Low + Range)) < Top)
				{

				}
				else if (Range < Bottom)
				{
					Range = -Low & (Bottom - 1);
				}
				else
				{
					break;
				}
 
				if (!m_pStream->WriteSafe(byte_t((Low >> nValueBits) & 0xFF)))
					return false;

				Range <<= 8;
				Low <<= 8;
			}*/


			return true;
		}

		virtual bool EncodeFinish()
		{
			for (int i = 0; i < _nValueBits / 8; i++)
			{

				if (!m_pStream->WriteSafe((byte_t)((Low >> nValueBits) & 0xFF)))
					return false;

				Low <<= 8;
			}
			return true;
		}


		virtual uint32_t GetAdditionalSize() const
		{
			return sizeof(uint64_t);
		}


	private:
		IWriteStream* m_pStream;
	};



	template<class _TCodeValue, uint16_t _nValueBits>
	class TRangeDecoder : public BaseACDecoder<_TCodeValue>
	{

		static const uint16_t  nValueBits = _nValueBits - 8;
		static const _TCodeValue Top = (_TCodeValue)1 << nValueBits;
		static const _TCodeValue Bottom = (_TCodeValue)1 << (nValueBits - 8);
	public:

		static const _TCodeValue MaxRange = Bottom;
		typedef _TCodeValue TCodeValue;

		TRangeDecoder() : m_pStream(nullptr), Low(0), Range((_TCodeValue)-1), m_nValue(0)
		{


		}
		void SetStream(IReadStream* pStream)
		{
			m_pStream = pStream;
			Range = (_TCodeValue)-1;
			Low = 0;
			m_nValue = 0;

		}
		virtual void StartDecode()
		{
			for (int i = 0; i < _nValueBits / 8; i++)
			{
				m_nValue = (m_nValue << 8) | m_pStream->ReadByte();
			}
		}

		virtual void DecodeSymbol(_TCodeValue SymbolLow, _TCodeValue SymbolHigh, _TCodeValue TotalRange)
		{
			Low += SymbolLow * Range;
			Range *= SymbolHigh - SymbolLow;

			/*while ((Low ^ Low + Range) < Top || Range < Bottom && ((Range = -Low & Bottom - 1), 1))
			{
				m_nValue = m_nValue << 8 | m_pStream->ReadByte(), Range <<= 8, Low <<= 8;
			}*/

			for (;;)
			{
				if ((Low ^ Low + Range) < Top)
				{

				}
				else if (Range < Bottom)
				{
					Range = -Low & (Bottom - 1);
				}
				else
				{
					break;
				}

				byte_t byte =m_pStream->ReadByte();

				m_nValue = (m_nValue << 8) | byte;
				Range <<= 8;
				Low <<= 8;

			}
		}

		virtual TCodeValue GetFreq(TCodeValue nTotalCount)
		{
			return (m_nValue - Low) / (Range /= nTotalCount);
		}

		byte_t ReadByte()
		{
			byte_t b = 0;
			m_pStream->ReadSafe(b);
			return b;
		}


	private:
		IReadStream* m_pStream;

		_TCodeValue m_nValue;
		_TCodeValue Low, Range;

	};


	typedef TRangeEncoder<uint64_t, 64> TRangeEncoder64;
	typedef TRangeDecoder<uint64_t, 64> TRangeDecoder64;

	typedef TRangeEncoder<uint32_t, 32> TRangeEncoder32;
	typedef TRangeDecoder<uint32_t, 32> TRangeDecoder32;


}
