#include "pch.h"
#include "ReadStreamPage.h"


namespace bptreedb
{
	namespace storage
	{

 

		TReadStreamPage::TReadStreamPage(IFilePagerPtr ptrIFilePager) :
				m_ptrIFilePager(ptrIFilePager)
		{

		}

		TReadStreamPage::~TReadStreamPage()
		{}

		void TReadStreamPage::Open(int64_t nPageAddr)
		{
			try
			{
				OpenNextPage(nPageAddr);
			}
			catch (CommonLib::CExcBase& excSrc)
			{
				excSrc.AddMsgT("Failed open ReadStreamPage addr: %1", nPageAddr);
				throw;
			}

		}

		std::streamsize TReadStreamPage::ReadBytes(byte_t* dst, size_t size)
		{
			read_bytes_impl(dst, size, false);
			return size;
		}

		std::streamsize TReadStreamPage::ReadInverse(byte_t* dst, size_t size)
		{
			read_bytes_impl(dst, size, true);
			return size;
		}


		bool TReadStreamPage::ReadBytesSafe(byte_t* dst, size_t size)
		{
			try
			{
				read_bytes_impl(dst, size, false);
				return true;
			}
			catch (...)
			{
			}

			return false;
		}

		bool TReadStreamPage::ReadInverseSafe(byte_t* dst, size_t size)
		{
			try
			{
				read_bytes_impl(dst, size, true);
				return true;
			}
			catch (...)
			{
			}

			return false;
		}

		bool TReadStreamPage::IsEnoughSpace(size_t size) const
		{
			return true;
		}

	 

		void TReadStreamPage::read_bytes_impl(byte_t* buffer, size_t size, bool bInvers)
		{
			try
			{
				size_t nPos = 0;
				while (size)
				{
					size_t nFreeSize = m_ptrStream->Size() - m_ptrStream->Pos();
					if (nFreeSize >= size)
					{
						if (bInvers)
							m_ptrStream->ReadInverse(buffer + nPos, size);
						else
							m_ptrStream->ReadBytes(buffer + nPos, size);

						size = 0;
					}
					else
					{
						size_t nReadSize = nFreeSize;
						if (nReadSize)
						{
							if (bInvers)
								m_ptrStream->ReadInverse(buffer + nPos, nReadSize);
							else
								m_ptrStream->ReadBytes(buffer + nPos, nReadSize);

							size -= nReadSize;
							nPos += nReadSize;
						}

						OpenNextPage(m_nNextAddr);
					}
				}

			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("ReadStreamPage: Failed read  size: %1", size, excSrc);
				throw;
			}

		}

		void TReadStreamPage::OpenNextPage(int64_t nPageAddr)
		{
			try
			{
				if (m_ptrCurrentPage.get() == nullptr)
					m_ptrCurrentPage = m_ptrIFilePager->ReadPage(nPageAddr);
				else
					m_ptrIFilePager->ReadPage(m_ptrCurrentPage, nPageAddr);

				m_ptrStream = m_ptrCurrentPage->GetReadStream();
				m_nNextAddr = m_ptrStream->ReadInt64();
				m_ptrStream->ReadInt64();
			}
			catch (std::exception& excSrc)
			{
				CommonLib::CExcBase::RegenExcT("ReadStreamPage: Failed openpage addr: %1", nPageAddr, excSrc);
			}
		}
 
	}
}
