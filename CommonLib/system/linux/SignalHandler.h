#pragma once
#include <signal.h>

namespace CommonLib {
    namespace system {
        namespace lin {

            class CSignalHandler
            {
            public:
                CSignalHandler( int signum, sighandler_t handler );
                ~CSignalHandler();
            private:
                int m_signum;
                sighandler_t m_prevHandler;
            };


        }
    }
}