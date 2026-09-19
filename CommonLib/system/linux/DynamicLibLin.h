#pragma once

namespace CommonLib {
    namespace system {
        namespace lin {

            class CDynamicLibraryLin
            {
            public:
                CDynamicLibraryLin(const astr& path);
                CDynamicLibraryLin(const wstr& path);
                ~CDynamicLibraryLin();

                CDynamicLibraryLin(const CDynamicLibraryLin&) = delete;
                CDynamicLibraryLin& operator=(const CDynamicLibraryLin&) = delete;

                void*  GetProcAddr(const astr& proc_name);
                void*  GetProcAddr(const wstr& proc_name);

                template <typename TFunkPtrType>
                void GetProcAddrEx(const astr& procName, TFunkPtrType& result)
                {
                    result = (TFunkPtrType)GetProcAddr(procName);
                }

                template <typename TFunkPtrType>
                void GetProcAddrEx(const wstr& procName, TFunkPtrType& result)
                {
                    result = (TFunkPtrType)GetProcAddr(procName);
                }
            private:
                void LoadLib(const astr& path);
            private:
                void* m_handle;
            };


        }
        typedef lin::CDynamicLibraryLin TDynamicLib;
        typedef std::shared_ptr<TDynamicLib> TDynamicLibPtr;
    }
}