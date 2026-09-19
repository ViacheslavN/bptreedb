#pragma once

#include "../alloc/simpleAlloc.h"
#include "../exception/exc_base.h"

namespace CommonLib {
    namespace Data {


        typedef std::shared_ptr<class CBlob> CBlobPtr;

        class CBlob
        {
        public:
            CBlob(IAllocPtr pAlloc = IAllocPtr() );
            CBlob(uint32_t nSize, IAllocPtr pAlloc = IAllocPtr());
            CBlob(const CBlob& blob);
            CBlob(CBlob&& blob);
            CBlob(byte* pBuf, uint32_t nSize, bool bAttach = true, IAllocPtr pAlloc = IAllocPtr() );
            CBlob& operator = (const CBlob& blob);
            CBlob& operator = (CBlob&& blob);
            ~CBlob();


            bool operator ==(const CBlob& blob) const;
            bool operator !=(const CBlob& blob) const;
            bool operator <(const CBlob& blob) const;
            bool operator >(const CBlob& blob) const;
            bool operator <=(const CBlob& blob) const;
            bool operator >=(const CBlob& blob) const;


            const unsigned char& operator [](size_t index) const;
            unsigned char& operator [](size_t index);

            int  Equals(const unsigned char *buffer, size_t _size = (size_t)-1) const;
            int  Equals(const CBlob& blob) const;

            int  Compare(const unsigned char *buffer, size_t _size = (size_t)-1) const;
            int  Compare(const CBlob& blob) const;

            void Attach(byte_t* pBuf, uint32_t nSize);
            byte_t* Deattach();
            void Copy(const byte_t* pBuf, uint32_t nSize);
            void Push_back(byte_t nVal);
            void Push_back(const byte_t* pBuf, uint32_t nSize);
            void  Resize(uint32_t nSize);
            void  Resize(uint32_t nSize, byte_t bVal);
            void  Resize_fill(uint32_t nSize, byte_t bVal);
            void  Reserve(uint32_t nSize, bool bClear = true);
            void  Reserve_fill(uint32_t nSize, byte_t bVal);
            bool   Empty() const;
            uint32_t Size() const;
            uint32_t Capacity() const;

            const byte_t* Buffer() const;
            byte_t* Buffer();

            void Clear(bool bDel = false);
        private:
            byte_t* m_pBuffer;
            IAllocPtr m_pAlloc;

            uint32_t m_nSize;
            uint32_t m_nCapacity;
            bool m_bAttach;
        };

    }
}