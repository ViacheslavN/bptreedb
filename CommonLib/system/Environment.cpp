#include "stdafx.h"
#include "Environment.h"


namespace CommonLib {
    namespace system {

        void CEnvironment::SetVariable(const std::string &varName, const std::string &value)
        {
#ifdef _WIN32
            std::string variable = str_format::StrFormatSafe("{0}={1}", varName, value);
            _putenv(variable.c_str());
#else
	        setenv(varName.c_str(), value.c_str(), 1);
#endif
        }

        void CEnvironment::RemoveVariable(const std::string& varName)
        {
#ifdef _WIN32
            std::string variable = str_format::StrFormatSafe("{0}=", varName);
            _putenv(variable.c_str());
#else
            unsetenv(varName.c_str());
#endif
        }

        CEnvValue::CEnvValue(const std::string& name, const std::string& value) :
            m_name(name), m_value(value)
        {
            m_systemParam = str_format::StrFormatSafe("{0}={1}", name, value);
        }

        const std::string& CEnvValue::GetName() const
        {
            return  m_name;
        }

        const std::string& CEnvValue::GetValue() const
        {
            return  m_value;
        }

        CEnvValue::operator const char*() const
        {
            return m_systemParam.c_str();
        }

    }

}