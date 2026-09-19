#pragma once

#include "zlib.h"

namespace CommonLib
{

	class CZStream
	{
	public:
		CZStream();
		~CZStream();

		void AttachOut(byte_t *pData, size_t nSize);
		void AttachIn(const byte_t *pData, size_t nSize);

		uint32_t GetAvailIn() const;
		uint32_t GetAvailOut() const;
	protected:
		z_stream m_strm;
	};

	/*

	class CZStream
	{
		public:
			CZStream(int level);
			CZStream();
			~CZStream();
			
			void AttachOut(byte_t *pData, size_t nSize);
			void AttachIn(const byte_t *pData, size_t nSize);
			int Deflate(int flush);
			int Inflate(int flush);
			void DeflatePending(unsigned *pending,
				int *bits);

			uint32_t GetAvailIn() const;
			uint32_t GetAvailOut() const;
			uint32_t GetTotalOut() const;
			uint32_t GetTotalIn() const;
		private:
			z_stream m_strm;
			bool m_encode;
	};*/
}