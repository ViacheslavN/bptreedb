#include "../../exception/PosixExc.h"
#include "LinPipe.h"
#include <sys/poll.h>

#define READ_PIPE 0
#define WRITE_PIPE 1

namespace CommonLib {
    namespace system {
        namespace lin {

            CLinPipe::CLinPipe(bool pollin, int timeoutms) : m_bPollin(pollin), m_timeoutms(timeoutms),m_bClosed(false)
            {
                if ( ::pipe( m_fds ) == -1 )
                    throw CPosixExc( "Failed to create pipe.", errno );

                if(fcntl(m_fds[READ_PIPE], F_SETFD, fcntl(m_fds[READ_PIPE], F_GETFD, 0) | FD_CLOEXEC) == -1)
                     throw CPosixExc(errno, "Filed set FD_CLOEXEC for read pipe");

                if(fcntl(m_fds[WRITE_PIPE], F_SETFD, fcntl(m_fds[WRITE_PIPE], F_GETFD, 0) | FD_CLOEXEC) == -1)
                    throw CPosixExc( errno, "Filed set FD_CLOEXEC for wtite pipe");

            }

            CLinPipe::~CLinPipe()
            {
                Close();
            }

            std::streamsize CLinPipe::Write(const byte_t* dataPtr, size_t dataSize)
            {
                const byte_t* bufferForWrite = reinterpret_cast< const byte_t* >(dataPtr);
                ssize_t  remainingDataSize = ( ssize_t )dataSize;
                std::streamsize numberOfBytesWritten = 0;

                while (!m_bClosed) {
                    if (remainingDataSize <= 0)
                        break;

                    ssize_t writeRes = write(m_fds[1], bufferForWrite, remainingDataSize);
                    if (-1 == writeRes)
                    {
                        if (EINTR == errno)
                            continue;
                        else
                            throw CPosixExc("Failed to write data into pipe.", errno);
                    }

                    bufferForWrite += writeRes;
                    remainingDataSize -= writeRes;
                    numberOfBytesWritten += writeRes;

                }

                return numberOfBytesWritten;
            }

            std::streamsize CLinPipe::Read(byte_t* dataPtr, size_t dataSize)
            {
                byte_t* bufferForRead = reinterpret_cast< byte_t* >(dataPtr);
                ssize_t  remainingDataSize = ( ssize_t )dataSize;
                size_t   numberOfBytesRead = 0;

                ssize_t readRes = 0;
                for ( ;; )
                {
                    if ( remainingDataSize <= 0 )
                       break;


                    if(!m_bPollin)
                    {
                        readRes = ::read(GetReadFd(), bufferForRead, remainingDataSize);
                        if (0 == readRes)
                            break;

                        else if (-1 == readRes)
                        {
                            if (EINTR == errno)
                                continue;
                            else
                                throw CPosixExc("Failed to read data from pipe.", errno);
                        }
                    }
                    else
                    {
                        struct pollfd fds;
                        fds.fd = GetReadFd();
                        fds.events = POLLIN;
                        fds.revents = 0;

                        int ret = ::poll(&fds, 1, m_timeoutms);
                        if (-1 == ret)
                           throw CPosixExc("Failed to poll pipe", errno);

                        else if (0 == ret)
                            break;
                        else
                        {
                           if ((fds.revents &POLLIN ) || (fds.revents &POLLHUP ))
                           {
                               readRes = ::read(GetReadFd(), bufferForRead, remainingDataSize);
                                if (0 == readRes)
                                     break;
                                else if (-1 == readRes)
                                    throw CPosixExc("Failed to read data from pipe.", errno);
                            }
                        }
                    }

                    bufferForRead += readRes;
                    remainingDataSize -= readRes;
                    numberOfBytesRead += readRes;

                }

                return numberOfBytesRead;

            }

            int CLinPipe::GetReadFd() const
            {
                return m_fds[READ_PIPE];
            }

            int CLinPipe::GetWriteFd() const
            {
                return m_fds[WRITE_PIPE];
            }

            void CLinPipe::Close()
            {
                CloseWriteEnd();
                CloseReadEnd();
                m_bClosed = true;
            }

            void CLinPipe::CloseWriteEnd()
            {
                if ( m_fds[WRITE_PIPE] != -1 )
                {
                    close( m_fds[WRITE_PIPE] );
                    m_fds[WRITE_PIPE] = -1;
                }
            }

            void CLinPipe::CloseReadEnd()
            {
                if ( m_fds[READ_PIPE] != -1 )
                {
                    close( m_fds[READ_PIPE] );
                    m_fds[READ_PIPE] = -1;
                }
            }

            void CLinPipe::AttachToStdout()
            {
                if ( dup2( m_fds[1], STDOUT_FILENO ) == -1 ) // std out
                    throw CPosixExc( "Failed to attach pipe to stdout.", errno );
            }

            void CLinPipe::AttachToStderr()
            {
                if ( dup2( m_fds[1], STDERR_FILENO ) == -1 ) // std err
                    throw CPosixExc( "Failed to attach pipe to stderr.", errno );
            }

            void CLinPipe::AttachToStdin()
            {
                if ( dup2( m_fds[0], STDIN_FILENO ) == -1 ) // std in
                    throw CPosixExc( "Failed to attach pipe to stdin.", errno );
            }


            LinPipePtr CLinPipe::Create(bool pollin, int timeoutms )
            {
                return std::make_shared<CLinPipe>(pollin, timeoutms);
            }


        }
    }
}