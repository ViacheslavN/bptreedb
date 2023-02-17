#include "pch.h"
#include "WriteStreamPage.h"

namespace bptreedb
{
	namespace storage
	{
		TWriteStreamPage::TWriteStreamPage(IFilePagerPtr ptrIFilePager) :
			m_ptrIFilePager(ptrIFilePager)
		{

		}

		TWriteStreamPage::~TWriteStreamPage()
		{

		}

		void TWriteStreamPage::Open(int64_t nPageAddr, bool bCreate)
		{
			try
			{
				OpenNextPage(nPageAddr, bCreate);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("Failed open WriteStreamPage addr: %1", nPageAddr, excSrc);
				throw;
			}

		}


		std::streamsize TWriteStreamPage::WriteBytes(const byte_t* buffer, size_t size)
		{
			write_bytes_impl(buffer, size, false);
			return size;
		}

		std::streamsize TWriteStreamPage::WriteInverse(const byte_t* buffer, size_t size)
		{
			write_bytes_impl(buffer, size, true);
			return size;
		}

		bool TWriteStreamPage::IsEnoughSpace(size_t size) const
		{
			return true;
		}

		void TWriteStreamPage::Close()
		{
			m_ptrStream->Seek(0, CommonLib::soFromBegin);
			m_ptrStream->Write(m_nNextAddr);
			m_ptrStream->Write(m_nPrevAddr);

			m_ptrIFilePager->SavePage(m_ptrCurrentPage);
		}
 
		void TWriteStreamPage::write_bytes_impl(const byte_t* buffer, size_t size, bool bInvers)
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
							m_nNextAddr = m_ptrIFilePager->GetNewFilePageAddr();

						OpenNextPage(m_nNextAddr, nNext == -1);
					}

				}
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("WritePageStream: Failed write bytes, size %1 ", size, excSrc);
				throw;
			}

		}

		void TWriteStreamPage::OpenNextPage(int64_t nPageAddr, bool bCreate)
		{
			try
			{

				if (m_ptrCurrentPage.get() != nullptr)
				{
					m_ptrStream->Seek(0, CommonLib::soFromBegin);
					m_ptrStream->Write(nPageAddr);
					m_ptrStream->Write(m_nPrevAddr);

					m_ptrIFilePager->SavePage(m_ptrCurrentPage);
					m_nPrevAddr = m_ptrCurrentPage->GetAddr();
				}


				if (bCreate)
					m_ptrCurrentPage = m_ptrIFilePager->GetNewPage(nPageAddr);
				else
				{
					if (m_ptrCurrentPage.get() == nullptr)
						m_ptrCurrentPage = m_ptrIFilePager->ReadPage(nPageAddr);
					else
						m_ptrIFilePager->ReadPage(m_ptrCurrentPage, nPageAddr);
				}
				

				m_nNextAddr = -1;

				if (!bCreate)
				{

					CommonLib::IMemoryReadStreamPtr  ptrStream = m_ptrCurrentPage->GetReadStream();
					m_nNextAddr = ptrStream->ReadInt64();
					int64_t nPrev = ptrStream->ReadInt64();

					if (nPrev != m_nPrevAddr)
						throw CommonLib::CExcBase("WriteStreamPage: Failed open page addr: %1  prev: %2, page prev: %3", nPageAddr, m_nPrevAddr, nPrev);

					m_nPrevAddr = nPrev;

				}

				m_ptrStream = m_ptrCurrentPage->GetWriteStream();
				m_ptrStream->Seek(2 * sizeof(int64_t), CommonLib::soFromBegin);
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("WriteStreamPage: Failed open page addr: %1", nPageAddr, excSrc);
				throw;
			}

		}
	}
}