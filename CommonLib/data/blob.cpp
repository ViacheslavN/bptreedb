#include "stdafx.h"
#include "blob.h"


namespace CommonLib {
    namespace Data {

        CBlob::CBlob(IAllocPtr pAlloc) : m_pAlloc(pAlloc)
                ,m_pBuffer(NULL), m_nSize(0), m_nCapacity(0), m_bAttach(false)
        {
            if(m_pAlloc.get() == nullptr)
                m_pAlloc = std::make_shared<CSimpleAlloc>();
        }
        CBlob::CBlob(uint32_t nSize, IAllocPtr pAlloc) : m_pAlloc(pAlloc)
                ,m_pBuffer(NULL), m_nSize(0), m_nCapacity(0), m_bAttach(false)
        {
            if(m_pAlloc.get() == nullptr)
                m_pAlloc = std::make_shared<CSimpleAlloc>();

            Resize(nSize);
        }
        CBlob::CBlob(const CBlob& blob) : m_bAttach(false) , m_nCapacity(0), m_nSize(0), m_pBuffer(nullptr)
        {
           m_pAlloc = blob.m_pAlloc;
           Copy(blob.Buffer(), blob.Size());
        }


        CBlob::CBlob( CBlob&& blob)
        {

            m_pAlloc = blob.m_pAlloc;
            m_pBuffer = blob.m_pBuffer;
            m_nSize = blob.m_nSize;
            m_nCapacity = blob.m_nCapacity;

            m_bAttach = blob.m_bAttach;
            if (!m_bAttach)
            {
                blob.m_pBuffer = nullptr;
                blob.m_nSize = 0;
                blob.m_nCapacity = 0;
            }

        }

        CBlob::CBlob(byte* pBuf, uint32_t nSize, bool bAttach,  IAllocPtr pAlloc) :
                m_pAlloc(pAlloc), m_bAttach(bAttach), m_nSize(nSize), m_nCapacity(nSize)
        {
            if(m_pAlloc.get() == nullptr)
                m_pAlloc = std::make_shared<CSimpleAlloc>();

            if(m_bAttach)
                m_pBuffer = pBuf;
            else if(m_pBuffer && m_nSize != 0)
            {
                m_pBuffer = (byte*)m_pAlloc->Alloc(m_nSize);
                memcpy(m_pBuffer, pBuf, m_nSize);
            }
        }

        CBlob& CBlob::operator = (const CBlob& blob)
        {
            Clear(true);

            m_pAlloc = blob.m_pAlloc;
            m_bAttach = false;
            Copy(blob.Buffer(), blob.Size());
            return *this;
        }

        CBlob& CBlob::operator = (CBlob&& blob)
        {
            Clear(true);

           m_pAlloc = blob.m_pAlloc;

            m_pBuffer = blob.m_pBuffer;
            m_nSize = blob.m_nSize;
            m_nCapacity = blob.m_nCapacity;

            m_bAttach = blob.m_bAttach;
            if (!m_bAttach)
            {
                blob.m_pBuffer = nullptr;
                blob.m_nSize = 0;
                blob.m_nCapacity = 0;
            }
            return *this;
        }

        CBlob::~CBlob()
        {
            Clear(true);
        }

        void CBlob::Attach(byte* pBuf, uint32_t nSize)
        {
            Clear(true);
            m_bAttach = true;
            m_pBuffer = pBuf;
            m_nSize = nSize;
            m_nCapacity = m_nSize;

        }
        byte* CBlob::Deattach()
        {
            byte *pBuf = m_pBuffer;
            m_nCapacity = 0;
            m_nSize = 0;
            m_pBuffer = NULL;
            m_bAttach = false;


            return pBuf;
        }
        void CBlob::Copy(const byte* pBuf, uint32_t nSize)
        {

            Resize(nSize);
            memcpy(m_pBuffer, pBuf, nSize);
        }
        void CBlob::Push_back(byte nVal)
        {
            if(Size() + 1 > m_nCapacity)
            {
                Reserve((Size() * 2) + 1, false);
            }

            m_pBuffer[m_nSize] = nVal;
            m_nSize++;
        }
        void CBlob::Push_back(const byte* pBuf, uint32_t nSize)
        {
            if(Size() + nSize > m_nCapacity)
            {
                Reserve(((Size() + nSize) * 2 ) + 1, false);
            }
            memcpy(m_pBuffer + m_nSize, pBuf, nSize);
            m_nSize += nSize;
        }
        bool CBlob::operator ==(const CBlob& blob) const
        {
            return Compare(blob) == 0;
        }

        bool CBlob::operator !=(const CBlob& blob) const
        {
            return Compare(blob) != 0;
        }

        bool CBlob::operator <(const CBlob& blob) const
        {
            return Compare(blob) < 0;
        }

        bool CBlob::operator >(const CBlob& blob) const
        {
            return Compare(blob) > 0;
        }

        bool CBlob::operator <=(const CBlob& blob) const
        {
            return Compare(blob) <= 0;
        }

        bool CBlob::operator >=(const CBlob& blob) const
        {
            return Compare(blob) >= 0;
        }
        const unsigned char& CBlob::operator [](size_t index) const
        {
            return *(Buffer() + index);
        }

        unsigned char& CBlob::operator [](size_t index)
        {
            return Buffer()[index];
        }
        int CBlob::Equals(const unsigned char *buffer, size_t _size) const
        {
            return Compare(buffer, _size) == 0;
        }

        int CBlob::Equals(const CBlob& blob) const
        {
            return Compare(blob) == 0;
        }

        int CBlob::Compare(const unsigned char *buffer, size_t _size) const
        {
            if(buffer == NULL)
                return Empty() ? 0 : 1;

            if(Empty())
                return -1;

            size_t minsize = min(Size(), _size);

            int n = memcmp(m_pBuffer, buffer, minsize);
            if(n != 0 || Size() == _size)
                return n;

            return Size() < _size ? -1 : 1;
        }

        int CBlob::Compare(const CBlob& blob) const
        {
            if(blob.Empty())
                return Empty() ? 0 : 1;

            return Compare(blob.Buffer(), blob.Size());
        }
        void  CBlob::Reserve(uint32_t nSize, bool bClear)
        {
            if(m_bAttach)
                Deattach();

            if(m_nCapacity < nSize)
            {
                byte* pBuffer = (byte*)m_pAlloc->Alloc(nSize);
                if(m_pBuffer != NULL)
                {
                    if(!bClear)
                        memcpy(pBuffer, m_pBuffer, m_nSize);
                    m_pAlloc->Free(m_pBuffer);
                }
                m_pBuffer = pBuffer;
                m_nCapacity = nSize;
            }
            if(bClear)
                m_nSize = 0;
        }
        void  CBlob::Reserve_fill(uint32_t nSize, byte nVal)
        {
            Reserve(nSize, false);
            uint32_t nLen = Capacity() - Size();
            if(nLen > 0)
                memset(m_pBuffer + m_nSize, nVal, nLen);

        }
        void  CBlob::Resize(uint32_t nSize)
        {
            if(m_bAttach)
                Deattach();
            if(m_nCapacity >= nSize)
            {
                m_nSize = nSize;
                return;
            }

            byte* pBuffer = (byte*)m_pAlloc->Alloc(nSize);
            if(m_pBuffer != NULL)
            {
                memcpy(pBuffer, m_pBuffer, m_nSize);
                m_pAlloc->Free(m_pBuffer);
            }

            m_nCapacity = nSize;
            m_nSize = nSize;

            m_pBuffer = pBuffer;

        }
        void  CBlob::Resize(uint32_t nSize, byte bVal)
        {
            Resize(nSize);
            memset(m_pBuffer, bVal, m_nSize);
        }
        void   CBlob::Resize_fill(uint32_t nSize, byte bVal)
        {
            uint32_t nCurSize = m_nSize;
            Resize(nSize);
            if(nCurSize < m_nSize)
                memset(m_pBuffer + nCurSize, bVal, m_nSize - nCurSize);
        }
        bool   CBlob::Empty() const
        {
            return m_nSize == 0;
        }

        uint32_t CBlob::Size() const
        {
            return m_nSize;
        }
        uint32_t CBlob::Capacity() const
        {
            return m_nCapacity;
        }
        const byte* CBlob::Buffer() const
        {
            return m_pBuffer;
        }
        byte* CBlob::Buffer()
        {
            return m_pBuffer;
        }
        void CBlob::Clear(bool bDel)
        {

            if(!m_bAttach && m_pBuffer != 0)
            {
                m_nSize = 0;
                if(bDel)
                {
                    m_nCapacity = 0;
                    m_pAlloc->Free(m_pBuffer);
                }
            }
        }

    }
}