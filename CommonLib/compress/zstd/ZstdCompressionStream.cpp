#include "stdafx.h"
#include "ZstdCompressionStream.h"
#include "../../ThirdParty/zstd/lib/zstd.h"
#include "../../exception/exc_base.h"
namespace CommonLib
{

	class CZStdHolder
	{
	public:
		CZStdHolder()
		{
			m_stream = ZSTD_createCStream();
			if (m_stream == nullptr)
				throw CExcBase("failed to create ZSTD_CStream");
		}

		~CZStdHolder()
		{
			if (m_stream != nullptr)
				ZSTD_freeCStream(m_stream);
		}


		void SetInputData(const byte_t *pData, size_t nSize)
		{
			m_input.pos = 0;
			m_input.src = pData;
			m_input.size = nSize;
		}

		void SetOutputData(byte_t *pData, size_t nSize)
		{
			m_output.pos = 0;
			m_output.dst = pData;
			m_output.size = nSize;
		}

	public:
		ZSTD_CStream* m_stream;
		ZSTD_inBuffer m_input;
		ZSTD_outBuffer m_output;
	};


	CZstdCompressionStream::CZstdCompressionStream(int compressionLevel)
	{
		m_pZstd.reset(new CZStdHolder());
		size_t  initResult = ZSTD_initCStream(m_pZstd->m_stream, compressionLevel);
		if (ZSTD_isError(initResult))
		{
			throw CExcBase("failed to create ZSTD_initCStream: {0}", ZSTD_getErrorName(initResult));
		}

	}
	 
	CZstdCompressionStream::~CZstdCompressionStream()
	{

	}

	void CZstdCompressionStream::AttachIn( const byte_t *pData, size_t nSize)
	{
		m_pZstd->SetInputData(pData, nSize);
	}

	void CZstdCompressionStream::AttachOut(byte_t *pData, size_t nSize)
	{
		m_pZstd->SetOutputData(pData, nSize);
	}

	void CZstdCompressionStream::Compress()
	{
 

		while (m_pZstd->m_input.pos < m_pZstd->m_input.size)
		{
			if (m_pZstd->m_output.pos == m_pZstd->m_output.size)
				return;

			size_t toRead = ZSTD_compressStream(m_pZstd->m_stream, &m_pZstd->m_output, &m_pZstd->m_input);
			if (ZSTD_isError(toRead))
			{
				throw CExcBase("Execute compressStream failed: {0}", ZSTD_getErrorName(toRead));

			}
		}
	}

	void CZstdCompressionStream::FinishCompress()
	{
		if (m_pZstd->m_output.pos == m_pZstd->m_output.size)
			return;

		size_t unflushed = ZSTD_endStream(m_pZstd->m_stream, &m_pZstd->m_output);
		if (ZSTD_isError(unflushed)) 
			throw CExcBase("ZSTD_endStream() error: {0}", ZSTD_getErrorName(unflushed));
		
	}

	uint32_t CZstdCompressionStream::GetAvailIn() const
	{
		return (uint32_t)(m_pZstd->m_input.size - m_pZstd->m_input.pos);
	}

	uint32_t CZstdCompressionStream::GetAvailOut() const
	{
		return (uint32_t)(m_pZstd->m_output.size - m_pZstd->m_output.pos);
	}
	
}