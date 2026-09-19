#include "stdafx.h"
#include "StdLineInOut.h"
#include "../str/StringEncoding.h"

namespace CommonLib {
    namespace io {

        StdLineInOut::StdLineInOut( std::istream& inStream, std::ostream& outStream ) :
                m_inStream( inStream ), m_outStream( outStream )
        {
        }

        bool StdLineInOut::IsEof()
        {
            return m_inStream.eof();
        }


        std::string StdLineInOut::GetLine()
        {
            std::string retVal;
            if(!std::getline( m_inStream, retVal ))
                throw CExcBase("Failed to read from the instream");

            return retVal;
        }

        void StdLineInOut::Write(const std::string& val)
        {
            m_outStream << val;
        }

        void StdLineInOut::WriteLine( const std::string& val )
        {
            m_outStream << val << std::endl;
        }
#ifdef WIN32

        WstdLineInOut::WstdLineInOut(std::wistream& inStream, std::wostream& outStream) :
                m_wInStream(inStream), m_wOutStream(outStream)
        {

        }

        WstdLineInOut::~WstdLineInOut()
        {

        }

        bool WstdLineInOut::IsEof()
        {
            return m_wInStream.eof();
        }

        std::string WstdLineInOut::GetLine()
        {
            std::wstring wStr;
            m_wInStream.sync();
            std::getline(m_wInStream, wStr);
            return StringEncoding::str_w2utf8_safe(wStr);
        }

        void WstdLineInOut::WriteLine(const std::string& val)
        {
            std::wstring line = StringEncoding::str_utf82w_safe(val + "\n");
            m_wOutStream << line << std::flush;
        }

        void WstdLineInOut::Write(const std::string& val)
        {
            std::wstring line = StringEncoding::str_utf82w_safe(val);
            m_wOutStream << line << std::flush;
        }


#endif

    }
}
