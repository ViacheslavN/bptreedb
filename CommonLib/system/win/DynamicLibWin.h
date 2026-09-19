#pragma once

namespace CommonLib
{
namespace system
{
namespace win
{

	class CDynamicLibraryWin
	{
	public:
		CDynamicLibraryWin(const std::string& path);
		CDynamicLibraryWin(const std::wstring& path);
		~CDynamicLibraryWin();

		CDynamicLibraryWin(const CDynamicLibraryWin&) = delete;
		CDynamicLibraryWin& operator=(const CDynamicLibraryWin&) = delete;

		FARPROC  GetProcAddr(const std::string& proc_name);
		FARPROC  GetProcAddr(const std::wstring& proc_name);

		template <typename TFunkPtrType>
		void GetProcAddrEx(const std::string& procName, TFunkPtrType& result)
		{
			result = (TFunkPtrType)GetProcAddr(procName);
		}

		template <typename TFunkPtrType>
		void GetProcAddrEx(const std::wstring& procName, TFunkPtrType& result)
		{
			result = (TFunkPtrType)GetProcAddr(procName);
		}
		
	private:
		void LoadLib(const std::wstring& path);
 

	private:
		HMODULE m_handle;
		
	};

}
    typedef win::CDynamicLibraryWin TDynamicLib;
    typedef std::shared_ptr<TDynamicLib> TDynamicLibPtr;

}
}