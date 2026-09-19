#include "stdafx.h"
#include "ZstdDecompressionStream.h"
#include "../../ThirdParty/zstd/lib/zstd.h"
#include "../../exception/exc_base.h"
namespace CommonLib
{

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment ( lib, "../ThirdParty/zstd/build/VS2010/bin/x64_Debug/libzstd_static.lib" )
#else
#pragma comment ( lib, "../ThirdParty/zstd/build/VS2010/bin/x64_Release/libzstd_static.lib" )
#endif

#else

#ifdef _DEBUG
#pragma comment ( lib, "../ThirdParty/zstd/build/VS2010/bin/Win32_Debug/libzstd_static.lib" )
#else
#pragma comment ( lib, "../ThirdParty/zstd/build/VS2010/bin/Win32_Release/libzstd_static.lib" )
#endif

#endif// _WIN64


	class ZDeStdHolder
	{
	public:
		ZDeStdHolder()
		{
			m_stream = ZSTD_createDStream();
			if (m_stream == nullptr)
				throw CExcBase("failed to create ZSTD_createDStream");
		}

		~ZDeStdHolder()
		{
			if (m_stream != nullptr)
				ZSTD_freeDStream(m_stream);
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
		ZSTD_DStream* m_stream;
		ZSTD_inBuffer m_input;
		ZSTD_outBuffer m_output;
	};


	CZstdDecompressionStream::CZstdDecompressionStream()
	{
		m_pZstd.reset(new ZDeStdHolder());
		size_t  initResult = ZSTD_initDStream(m_pZstd->m_stream);
		if (ZSTD_isError(initResult))
		{
			throw CExcBase("failed to create ZSTD_initDStream: {0}", ZSTD_getErrorName(initResult));
		}

	}

	CZstdDecompressionStream::~CZstdDecompressionStream()
	{

	}

	void CZstdDecompressionStream::AttachIn(const byte_t *pData, size_t nSize)
	{
		m_pZstd->SetInputData(pData, nSize);
	}

	void CZstdDecompressionStream::AttachOut(byte_t *pData, size_t nSize)
	{
		m_pZstd->SetOutputData(pData, nSize);
	}

	void CZstdDecompressionStream::Decompress()
	{


		while (m_pZstd->m_input.pos < m_pZstd->m_input.size)
		{
			if (m_pZstd->m_output.pos == m_pZstd->m_output.size)
				return;

			size_t toRead = ZSTD_decompressStream(m_pZstd->m_stream, &m_pZstd->m_output, &m_pZstd->m_input);
			if (ZSTD_isError(toRead))
			{
				throw CExcBase("Execute compressStream failed: {0}", ZSTD_getErrorName(toRead));

			}
		}
	}

	uint32_t CZstdDecompressionStream::GetAvailIn() const
	{
		return uint32_t(m_pZstd->m_input.size - m_pZstd->m_input.pos);
	}

	uint32_t CZstdDecompressionStream::GetAvailOut() const
	{
		return uint32_t(m_pZstd->m_output.size - m_pZstd->m_output.pos);
	}

}