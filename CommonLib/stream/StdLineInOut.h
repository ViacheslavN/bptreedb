#pragma once
#include "io.h"

namespace CommonLib {
    namespace io {

        class StdLineInOut : public ILineInOut
        {
        public:
            StdLineInOut( std::istream& inStream, std::ostream& outStream );
            virtual ~StdLineInOut() {}

            virtual bool IsEof();
            virtual std::string GetLine();
            virtual void Write(const std::string& val);
            virtual void WriteLine(const std::string& val);


        private:
            std::istream& m_inStream;
            std::ostream& m_outStream;
        };
#ifdef WIN32
        class WstdLineInOut : public ILineInOut
        {
        public:
            WstdLineInOut(std::wistream& inStream, std::wostream& outStream);
            virtual ~WstdLineInOut();

            virtual bool IsEof();
            virtual std::string GetLine();
            virtual void Write(const std::string& val);
            virtual void WriteLine(const std::string& val);

        private:
            std::wistream& m_wInStream;
            std::wostream& m_wOutStream;
        };
#endif
    }
}
