#pragma once

#include "Handle.h"
#include "../../stream/io.h"

namespace CommonLib {
    namespace system {
        namespace win {

            typedef std::shared_ptr<class WinPipe> WinPipePtr;

            class WinPipe : public io::IWriteRead
            {
            public:
                WinPipe(CHandlePtr read, CHandlePtr write);
                virtual ~WinPipe();

                virtual std::streamsize Write(const byte_t* dataPtr, size_t dataSize);
                virtual std::streamsize Read(byte_t* dataPtr, size_t dataSize);

                CHandlePtr GetReadFd() const;
                CHandlePtr GetWriteFd() const;
                void Close();
                void CloseWriteEnd();
                void CloseReadEnd();

                static WinPipePtr Create();

            private:
                CHandlePtr m_read;
                CHandlePtr m_write;
            };

        }
    }
}