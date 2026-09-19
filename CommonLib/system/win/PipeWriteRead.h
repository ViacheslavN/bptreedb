#pragma once

#include "../../stream/io.h"
#include "WinPipe.h"

namespace CommonLib {
    namespace system {
        namespace win {

        class PipeWriteRead : public io::IWriteRead
        {
        public:
            PipeWriteRead(WinPipePtr ptrReadPipe, WinPipePtr ptrWritePipe);
            ~PipeWriteRead();

            virtual std::streamsize Write(const byte_t* dataPtr, size_t dataSize);
            virtual std::streamsize Read(byte_t* dataPtr, size_t dataSize);

        private:
            WinPipePtr m_ptrReadPipe;
            WinPipePtr m_ptrWritePipe;


        };

        }
    }
}
