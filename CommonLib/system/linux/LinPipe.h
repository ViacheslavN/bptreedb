#pragma once

#include "../../stream/io.h"

namespace CommonLib {
    namespace system {
        namespace lin {

            typedef std::shared_ptr<class CLinPipe> LinPipePtr;

            class CLinPipe : public io::IWriteRead
            {
            public:
                CLinPipe(bool pollin = false, int timeoutms = -1);
                virtual ~CLinPipe();

                virtual std::streamsize Write(const byte_t* dataPtr, size_t dataSize);
                virtual std::streamsize Read(byte_t* dataPtr, size_t dataSize);

                int GetReadFd() const;
                int GetWriteFd() const;
                void Close();
                void CloseWriteEnd();
                void CloseReadEnd();

                void AttachToStdout();
                void AttachToStderr();
                void AttachToStdin();

                static LinPipePtr Create(bool pollin = false, int timeoutms = -1);

            private:
                int m_fds[2];
                bool m_bPollin;
                int m_timeoutms;
                volatile bool m_bClosed;
            };



        }
    }
}