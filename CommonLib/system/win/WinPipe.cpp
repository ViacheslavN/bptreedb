#include "stdafx.h"
#include "WinPipe.h"
#include "intsafe.h"

namespace CommonLib {
    namespace system {
        namespace win {

            WinPipe::WinPipe(CHandlePtr read, CHandlePtr write) : m_read(read), m_write(write)
            {

            }

            WinPipe::~WinPipe()
            {
                Close();
            }

            std::streamsize WinPipe::Write(const byte_t* dataPtr, size_t dataSize)
            {
                if ( dataPtr == nullptr|| dataSize == 0 )
                    throw CExcBase("WinPipe: Failed to write data, incorrect buffer received.");

                DWORD wordDataSize = 0;
                if ( ::SizeTToDWord( dataSize, &wordDataSize ) != S_OK )
                    throw CExcBase("WinPipe: Failed to write data, ailed to convert size from size_t to DWORD." );

                DWORD written = 0;
                if ( !::WriteFile( m_write->GetHandleExc(), dataPtr, wordDataSize, &written, NULL ) )
                {
                    DWORD lastError = ::GetLastError();
                    if ( ERROR_BROKEN_PIPE == lastError )
                        return 0;

                    throw CWinExc( "WinPipe: Failed to write data, ." );
                }

                return written;
            }

            std::streamsize WinPipe::Read(byte_t* dataPtr, size_t dataSize)
            {
                if ( dataPtr == nullptr|| dataSize == 0 )
                    throw CExcBase("WinPipe: Failed to read data, incorrect buffer received.");

                DWORD wordBufSize = 0;
                if ( ::SizeTToDWord( dataSize, &wordBufSize ) != S_OK )
                    throw CExcBase("WinPipe: Failed to read data, ailed to convert size from size_t to DWORD." );

                DWORD readed = 0;
                if ( !::ReadFile( m_read->GetHandleExc(), dataPtr, wordBufSize, &readed, NULL ) )
                {
                    DWORD lastError = ::GetLastError();
                    if ( ERROR_BROKEN_PIPE == lastError )
                        return 0;

                    throw CWinExc( "WinPipe: Failed to read data." );
                }

                return readed;
            }


            CHandlePtr WinPipe::GetReadFd() const
            {
                return m_read;
            }

            CHandlePtr WinPipe::GetWriteFd() const
            {
                return m_write;
            }

            void WinPipe::Close()
            {
                CloseReadEnd();
                CloseWriteEnd();
            }

            void WinPipe::CloseWriteEnd()
            {
                m_write->Close();
            }

            void WinPipe::CloseReadEnd()
            {
                m_read->Close();
            }

            WinPipePtr WinPipe::Create()
            {
                try
                {
                    SECURITY_ATTRIBUTES attr;


                    attr.nLength = sizeof( SECURITY_ATTRIBUTES );
                    attr.bInheritHandle = TRUE;
                    attr.lpSecurityDescriptor = NULL;


                    HANDLE readEnd;
                    HANDLE writeEnd;

                    if ( !::CreatePipe( &readEnd, &writeEnd, &attr, 0 ) )
                        throw CWinExc("Failed to create pipe");

                    return std::make_shared<WinPipe>( std::make_shared<CHandle>(readEnd),std::make_shared<CHandle>(writeEnd) );
                }
                catch (std::exception& exc)
                {
                    CExcBase::RegenExcT("WinPipe: Failed to create.", exc);
                    throw;
                }
            }

        }
    }
}