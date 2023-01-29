#pragma once

#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/stream/stream.h"
#include "../../CommonLib/stream/StreamBaseEmpty.h"
#include "FilePage.h"

namespace bptreedb
{
	namespace storage
	{

		template<class _TPageIO>
		class TReadStreamPage : public CommonLib::TMemoryStreamBaseEmpty<CommonLib::IMemoryReadStream>
		{
		public:
			typedef _TPageIO TPageIO;
			typedef std::shared_ptr<TPageIO> TPageIOPtr;

			TReadStreamPage(TPageIOPtr ptrPageIO, CommonLib::IAllocPtr ptrAlloc) :
				m_ptrPageIO(ptrPageIO), m_ptrAlloc(ptrAlloc)
			{

			}

			void Open(int64_t nPageAddr, uint32_t nPageSize)
			{
				try
				{
					m_nPageSize = nPageSize;
					OpenNextPage(nPageAddr);

				}
				catch (CommonLib::CExcBase& excSrc)
				{
					excSrc.AddMsgT("Failed open ReadStreamPage addr: %1, pagesize: %2", nPageAddr, m_nPageSize);
					throw;
				}

			}

			virtual std::streamsize ReadBytes(byte_t* dst, size_t size)
			{
				read_bytes_impl(dst, size, false);
				return size;
			}

			virtual std::streamsize ReadInverse(byte_t* dst, size_t size)
			{
				read_bytes_impl(dst, size, true);
				return size;
			}


			virtual bool ReadBytesSafe(byte_t* dst, size_t size)
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

			virtual bool ReadInverseSafe(byte_t* dst, size_t size)
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

			virtual bool IsEnoughSpace(size_t size) const
			{
				return true;
			}

		private:

			void read_bytes_impl(byte_t* buffer, size_t size, bool bInvers)
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

			void OpenNextPage(int64_t nPageAddr)
			{
				try
				{
					if(m_pCurrentPage.get() == nullptr)
						m_pCurrentPage = CFilePage::Read(m_ptrPageIO, m_ptrAlloc, m_nPageSize, nPageAddr);
					else
						CFilePage::Read(m_ptrPageIO, m_pCurrentPage, nPageAddr);

					m_ptrStream = m_pCurrentPage->GetReadStream();
					m_nNextAddr = m_ptrStream->ReadInt64();
					m_ptrStream->ReadInt64();
				}
				catch (std::exception& excSrc)
				{
					CommonLib::CExcBase::RegenExcT("ReadStreamPage: Failed openpage  addr: %1, pagesize: %2", nPageAddr, m_nPageSize, excSrc);
				}
			}

		private:
			int64_t m_nNextAddr{ -1 };
			uint32_t m_nPageSize{ 0 };
			TPageIOPtr m_ptrPageIO;
			CFilePagePtr m_pCurrentPage;
			CommonLib::IAllocPtr m_ptrAlloc;
			CommonLib::IMemoryReadStreamPtr m_ptrStream;
		};
	}
}
