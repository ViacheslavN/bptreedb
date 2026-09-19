#pragma once



namespace CommonLib
{

	class ZDeStdHolder;
	typedef std::shared_ptr< ZDeStdHolder> TZDeStdHolder;

	class CZstdDecompressionStream
	{
	public:
		CZstdDecompressionStream();
		~CZstdDecompressionStream();

		void AttachOut(byte_t *pData, size_t nSize);
		void AttachIn(const byte_t *pData, size_t nSize);

		void Decompress();

		uint32_t GetAvailIn() const;
		uint32_t GetAvailOut() const;

	private:
		TZDeStdHolder m_pZstd;
	};

}