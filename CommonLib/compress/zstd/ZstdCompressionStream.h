#pragma once



namespace CommonLib
{

	class CZStdHolder;
	typedef std::shared_ptr< CZStdHolder> TZStdHolder;
	
	class CZstdCompressionStream
	{
		public:
			CZstdCompressionStream(int compressionLevel);
			~CZstdCompressionStream();

			void AttachOut(byte_t *pData, size_t nSize);
			void AttachIn( const byte_t *pData, size_t nSize);

			void Compress();
			void FinishCompress();

			uint32_t GetAvailIn() const;
			uint32_t GetAvailOut() const;


		private:
			TZStdHolder m_pZstd;
	};

}