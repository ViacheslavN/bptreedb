#pragma once
#include "../../../../CommonLib/stream/MemoryStream.h"
#include "../../../../CommonLib/stream/FixedMemoryStream.h"
#include "../CompressUtils.h"


namespace bptreedb
{

	class CBoolEncoder
	{

	public:
		CBoolEncoder();
		~CBoolEncoder();

		void Clear();

		void AddSymbol(bool bSign);
		void RemoveSymbol(bool bSign);
		uint32_t GetCompressSize() const;

		bool BeginEncoding(CommonLib::IWriteStream *pStream);
		void EncodeBit(bool bit, uint32_t pos);


		void BeginDecoding(CommonLib::IReadStream *pStream);
		bool DecodeBit(uint32_t pos);

	private:

		class IBoolEncoder
		{
		public:
				IBoolEncoder(){}
				virtual ~IBoolEncoder(){}

				virtual bool BeginEncoding(CommonLib::IWriteStream *pStream) = 0;
				virtual void EncodeSymbol(uint32_t pos) = 0;

				virtual void BeginDecoding(CommonLib::IReadStream *pStream) = 0;
				virtual bool DecodeSymbol(uint32_t pos) = 0;
		};

	private:
		enum EncodeType
		{
			OneBit = 0,
			BitEncode = 1,
			PosBits =2
		};

		enum  BitsOffset
		{
			SignBit = 2,
			SignPosBit = 3,
			DataTypePosBit = 4
		};


		class  CBitsRW
		{
					   
		public:

			void WriteBit(bool bit)
			{
				if (bit)
				{
					uint32_t byte_offset = m_bit_pos >> 3;
					m_pBuffer[byte_offset] |= ((byte_t)1 << (m_bit_pos & 0x7));
				}

				m_bit_pos += 1;			
			}

			bool ReadBit()
			{
				uint32_t byte_offset = m_bit_pos >> 3; 
				return (m_pBuffer[byte_offset] >> (m_bit_pos++ & 0x7)) & 0x1;
			}


			void Attach(byte_t* pBuffer)
			{
				m_pBuffer = pBuffer;
				m_bit_pos = 0;
			}


			void Clear()
			{
				m_pBuffer = nullptr;
				m_bit_pos = 0;
			}

			byte_t* m_pBuffer{ nullptr };
			uint32_t m_bit_pos{0};
			 
		};


		EncodeType m_encodeType;
		eCompressDataType m_dataType;
		bool m_bit;
		uint32_t m_bools[2];
		std::vector<uint32_t> m_pos;
		CBitsRW m_bitsRW;
		CommonLib::CFxMemoryWriteStream m_WriteStream;
	};


}