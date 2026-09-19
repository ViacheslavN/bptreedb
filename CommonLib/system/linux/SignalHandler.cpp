#include "stdafx.h"
#include "SignalHandler.h"
#include "../../exception/PosixExc.h"

namespace CommonLib {
    namespace system {
        namespace lin {

            CSignalHandler::CSignalHandler( int signum, sighandler_t handler ) : m_signum( signum )
            {
                m_prevHandler = signal( m_signum, handler );
                if( m_prevHandler == SIG_ERR )
                    throw CPosixExc("Failed to set signal handler.", errno);
            }

            CSignalHandler::~CSignalHandler()
            {
                signal( m_signum, m_prevHandler );
            }


        }
    }
}