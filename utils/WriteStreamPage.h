#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/stream/FixedMemoryStream.h"
#include "../../CommonLib/stream/MemoryStream.h"
#include "../../CommonLib/stream/StreamBaseEmpty.h"
#include "../storage/PageObject.h"

namespace bptreedb
{
	namespace utils
	{

		template<class _TStorage>
		class TWriteStreamPage : public CommonLib::TMemoryStreamBaseEmpty<CommonLib::IMemoryWriteStream>
		{
		public:
			typedef _TStorage TStorage;
			typedef std::shared_ptr<TStorage> TStoragePtr;

			TWriteStreamPage(TStoragePtr& pStorage) :
				m_pStorage(pStorage)
			{

			}

			void Open(int64_t nPageAddr, uint32_t nPageSize, bool bCreate)
			{
				try
				{
					m_nPageSize = nPageSize;
					OpenNextPage(nPageAddr, bCreate);
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("Failed open WriteStreamPage addr: %1, pagesize: %2", nPageAddr, m_nPageSize, excSrc);
					throw;
				}

			}


			virtual std::streamsize WriteBytes(const byte_t* buffer, size_t size)
			{
				write_bytes_impl(buffer, size, false);
				return size;
			}

			virtual std::streamsize WriteInverse(const byte_t* buffer, size_t size)
			{
				write_bytes_impl(buffer, size, true);
				return size;
			}

			virtual bool IsEnoughSpace(size_t size) const
			{
				return true;
			}

			virtual void Close()
			{

				m_stream.Seek(0, CommonLib::soFromBegin);
				m_stream.Write(m_nNextAddr);
				m_stream.Write(m_nPrevAddr);

				m_pStorage->SaveFilePage(m_pCurrentPage);
			}

		private:
			void write_bytes_impl(const byte_t* buffer, size_t size, bool bInvers)
			{
				try
				{
					size_t nPos = 0;
					while (size)
					{
						size_t nFreeSize = m_stream.Size() - m_stream.Pos();
						if (size <= nFreeSize)
						{
							if (bInvers)
								m_stream.WriteInverse(buffer + nPos, size);
							else
								m_stream.WriteBytes(buffer + nPos, size);
							size = 0;
						}
						else
						{

							size_t nWriteSize = nFreeSize;
							if (bInvers)
								m_stream.WriteInverse(buffer + nPos, nWriteSize);
							else
								m_stream.WriteBytes(buffer + nPos, nWriteSize);

							size -= nWriteSize;
							nPos += nWriteSize;

							int64_t nNext = m_nNextAddr;
							if (m_nNextAddr == -1)
								m_nNextAddr = m_pStorage->GetNewFilePageAddr(m_nPageSize);

							OpenNextPage(m_nNextAddr, nNext == -1);
						}

					}
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("WritePageStream: Filed write bytes, size %1 ", size, excSrc);
					throw;
				}

			}


			void OpenNextPage(int64_t nPageAddr, bool bCreate)
			{
				try
				{
					FilePagePtr pPage;

					if (bCreate)
						pPage = m_pStorage->GetEmptyFilePage(nPageAddr, m_nPageSize);
					else
						pPage = m_pStorage->GetFilePage(nPageAddr, m_nPageSize);

					if (m_pCurrentPage.get() != nullptr)
					{
						m_stream.Seek(0, CommonLib::soFromBegin);
						m_stream.Write(pPage->GetPageAddr());
						m_stream.Write(m_nPrevAddr);

						m_pStorage->SaveFilePage(m_pCurrentPage);

						m_nPrevAddr = m_pCurrentPage->GetPageAddr();
					}

					m_pCurrentPage = pPage;
					m_nNextAddr = -1;

					if (!bCreate)
					{
						CommonLib::CReadMemoryStream stream;
						stream.AttachBuffer(m_pCurrentPage->GetData(), m_pCurrentPage->GetPageSize());

						m_nNextAddr = stream.ReadInt64();
						int64_t nPrev = stream.ReadInt64();

						if (nPrev != m_nPrevAddr)
							throw CommonLib::CExcBase("WriteStreamPage:  Failed openpage addr: %1  prev: %2, pageprev: %3", nPageAddr, m_nPrevAddr, nPrev);

						m_nPrevAddr = nPrev;

					}
					m_stream.AttachBuffer(m_pCurrentPage->GetData(), m_pCurrentPage->GetPageSize());
					m_stream.Seek(2 * sizeof(int64_t), CommonLib::soFromBegin);
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("WriteStreamPage: Failed openpage  addr: %1, pagesize: %2", nPageAddr, m_nPageSize, excSrc);
					throw;
				}

			}


		private:

			int64_t m_nNextAddr{ -1 };
			int64_t m_nPrevAddr{ -1 };

			uint32_t m_nPageSize{ 0 };
			TStoragePtr m_pStorage;
			FilePagePtr m_pCurrentPage;
			CommonLib::CFxMemoryWriteStream m_stream;
		};


	}
}