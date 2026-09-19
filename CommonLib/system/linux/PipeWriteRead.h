#pragma once

#include "../../stream/io.h"
#include "LinPipe.h"

namespace CommonLib {
    namespace system {
        namespace lin {

            class PipeWriteRead : public io::IWriteRead
            {
            public:
                PipeWriteRead(LinPipePtr ptrReadPipe, LinPipePtr ptrWritePipe);
                ~PipeWriteRead();

                virtual std::streamsize Write(const byte_t* dataPtr, size_t dataSize);
                virtual std::streamsize Read(byte_t* dataPtr, size_t dataSize);

            private:
                LinPipePtr m_ptrReadPipe;
                LinPipePtr m_ptrWritePipe;


            };

        }
    }
}
