#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/stream/FixedMemoryStream.h"
#include "../../CommonLib/stream/MemoryStream.h"
#include "../../CommonLib/stream/StreamBaseEmpty.h"
#include "../storage/PageObject.h"

namespace bptreedb
{
	namespace storage
	{

		template<class _TPageIO>
		class TWriteStreamPage : public CommonLib::TMemoryStreamBaseEmpty<CommonLib::IMemoryWriteStream>
		{
		public:
			typedef _TPageIO TPageIO;
			typedef std::shared_ptr<TPageIO> TPageIOPtr;

			TWriteStreamPage(TPageIOPtr ptrPageIO, std::shared_ptr<CommonLib::IAlloc> ptrAlloc, uint32_t objectID, uint32_t parentID, uint32_t parenttype) :
				m_ptrPageIO(ptrPageIO), m_objectID(objectID), m_parentID(parentID), m_parentType(parenttype), m_ptrAlloc(ptrAlloc)
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

				m_ptrStream->Seek(0, CommonLib::soFromBegin);
				m_ptrStream->Write(m_nNextAddr);
				m_ptrStream->Write(m_nPrevAddr);

				m_pCurrentPage->Save(m_ptrPageIO);
			}

		private:
			void write_bytes_impl(const byte_t* buffer, size_t size, bool bInvers)
			{
				try
				{
					size_t nPos = 0;
					while (size)
					{
						size_t nFreeSize = m_ptrStream->Size() - m_ptrStream->Pos();
						if (size <= nFreeSize)
						{
							if (bInvers)
								m_ptrStream->WriteInverse(buffer + nPos, size);
							else
								m_ptrStream->WriteBytes(buffer + nPos, size);
							size = 0;
						}
						else
						{

							size_t nWriteSize = nFreeSize;
							if (bInvers)
								m_ptrStream->WriteInverse(buffer + nPos, nWriteSize);
							else
								m_ptrStream->WriteBytes(buffer + nPos, nWriteSize);

							size -= nWriteSize;
							nPos += nWriteSize;

							int64_t nNext = m_nNextAddr;
							if (m_nNextAddr == -1)
								m_nNextAddr = m_ptrPageIO->GetNewFilePageAddr(m_nPageSize);

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
					CFilePagePtr ptrPage;

					if (bCreate)
						ptrPage = std::make_shared<CFilePage>(m_ptrAlloc, m_nPageSize, nPageAddr, m_objectID, (ObjectPageType)m_objectType, m_parentID, (ObjectPageType)m_parentType);
					else
					{
						if (ptrPage.get() == 0)
							ptrPage = CFilePage::Read(m_ptrPageIO, m_ptrAlloc, m_nPageSize, nPageAddr);
						else
							CFilePage::Read(m_ptrPageIO, ptrPage, nPageAddr);
					}

					if (m_pCurrentPage.get() != nullptr)
					{
						m_ptrStream->Seek(0, CommonLib::soFromBegin);
						m_ptrStream->Write(ptrPage->GetAddr());
						m_ptrStream->Write(m_nPrevAddr);

						m_pCurrentPage->Save(m_ptrPageIO);
						m_nPrevAddr = m_pCurrentPage->GetAddr();
					}

					m_pCurrentPage = ptrPage;
					m_nNextAddr = -1;

					if (!bCreate)
					{

						CommonLib::IMemoryReadStreamPtr  ptrStream = m_pCurrentPage->GetReadStream();
						m_nNextAddr = ptrStream->ReadInt64();
						int64_t nPrev = ptrStream->ReadInt64();

						if (nPrev != m_nPrevAddr)
							throw CommonLib::CExcBase("WriteStreamPage: Failed open page addr: %1  prev: %2, page prev: %3", nPageAddr, m_nPrevAddr, nPrev);

						m_nPrevAddr = nPrev;

					}

					m_ptrStream = m_pCurrentPage->GetWriteStream();
					m_ptrStream->Seek(2 * sizeof(int64_t), CommonLib::soFromBegin);
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("WriteStreamPage: Failed open page addr: %1, page size: %2", nPageAddr, m_nPageSize, excSrc);
					throw;
				}

			}


		private:
			std::shared_ptr<CommonLib::IAlloc> m_ptrAlloc;
			int64_t m_nNextAddr{ -1 };
			int64_t m_nPrevAddr{ -1 };

			uint32_t m_nPageSize{ 0 };
			TPageIOPtr m_ptrPageIO;
			CFilePagePtr m_pCurrentPage;
			CommonLib::IMemoryWriteStreamPtr m_ptrStream;

			uint32_t m_objectType{ eWritePageStream };
			uint32_t m_objectID{ 0 };
			uint32_t m_parentID{ 0 };
			uint32_t m_parentType{ 0 };
		};


	}
}