#include "stdafx.h"
#include "../../exception/WinExc.h"
#include "../../str/StringEncoding.h"
#include "DynamicLibWin.h"

namespace CommonLib
{
namespace system
{
namespace win
{

	CDynamicLibraryWin::CDynamicLibraryWin(const std::wstring& path) : m_handle(NULL)
	{
		LoadLib(path);
	}

	CDynamicLibraryWin::CDynamicLibraryWin(const std::string& path)
	{
		LoadLib(StringEncoding::str_a2w_safe(path));
	}
	 
	void CDynamicLibraryWin::LoadLib(const std::wstring& path)
	{
		m_handle = ::LoadLibraryW(path.c_str());
		if (m_handle == NULL)
			throw  CWinExc("Failed to load library {0}", StringEncoding::str_w2a_safe(path));
	}

	CDynamicLibraryWin::~CDynamicLibraryWin()
	{
		if(m_handle != 0)
			FreeLibrary(m_handle);
	}

	FARPROC  CDynamicLibraryWin::GetProcAddr(const std::string& proc_name)
	{
		FARPROC  pfunAddr = (FARPROC)GetProcAddress(m_handle, proc_name.c_str());
		if (pfunAddr == NULL)
			throw  CWinExc("Failed to get proc addr {0}", proc_name);

		return pfunAddr;
	}

	FARPROC  CDynamicLibraryWin::GetProcAddr(const std::wstring& proc_name)
	{
		return GetProcAddr(StringEncoding::str_w2a_safe(proc_name));
	}
}
}
}