#include "stdafx.h"
#include "PipeWriteRead.h"

namespace CommonLib {
    namespace system {
        namespace win {

            PipeWriteRead::PipeWriteRead(WinPipePtr ptrReadPipe, WinPipePtr ptrWritePipe) :
            m_ptrWritePipe(ptrWritePipe), m_ptrReadPipe(ptrReadPipe)
            {}

            PipeWriteRead::~PipeWriteRead()
            {}

            std::streamsize PipeWriteRead::Write(const byte_t* dataPtr, size_t dataSize)
            {
               return m_ptrWritePipe->Write(dataPtr, dataSize);
            }

            std::streamsize PipeWriteRead::Read(byte_t* dataPtr, size_t dataSize)
            {
                return m_ptrReadPipe->Read(dataPtr, dataSize);
            }

        }
    }
}
