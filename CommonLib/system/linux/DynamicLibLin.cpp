#include <dlfcn.h>
#include "stdafx.h"
#include "../../exception/PosixExc.h"
#include "../../str/StringEncoding.h"
#include "DynamicLibLin.h"


namespace CommonLib {
    namespace system {
        namespace lin {

            CDynamicLibraryLin::CDynamicLibraryLin(const astr& path)
            {
                LoadLib(path);
            }

            CDynamicLibraryLin::CDynamicLibraryLin(const wstr& path)
            {
                LoadLib(StringEncoding::str_w2utf8_safe(path));
            }

            CDynamicLibraryLin::~CDynamicLibraryLin()
            {
                if (m_handle != NULL)
                {
                    dlclose(m_handle);
                    m_handle = NULL;
                }
            }

            void* CDynamicLibraryLin::GetProcAddr(const astr& proc_name)
            {
                void* pfnAddr = dlsym(m_handle, proc_name.c_str());
                char* pszError = dlerror();
                if (pfnAddr == NULL || pszError != NULL)
                {
                    throw CExcBase("Failed to load function {0}, error: {1}", proc_name, pszError != nullptr ? pszError : "unknown error");
                }

                return pfnAddr;
            }

            void* CDynamicLibraryLin::GetProcAddr(const wstr& proc_name)
            {
                return GetProcAddr(StringEncoding::str_w2utf8_safe(proc_name));
            }

            void CDynamicLibraryLin::LoadLib(const astr& path)
            {
                m_handle = dlopen(path.c_str(), RTLD_LAZY);
                if (NULL == m_handle) {
                    char* pszError = dlerror();
                    throw CExcBase("Failed to load library {0}, error: {1}", path, pszError != nullptr ? pszError : "unknown error");
                }
            }
        }

    }
}
