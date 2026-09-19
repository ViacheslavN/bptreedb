#include "stdafx.h"
#include "Handle.h"

namespace CommonLib
{
	namespace system
	{
		namespace win
		{
			CHandle::CHandle(HANDLE handle) : m_handle(handle)
			{

			}

			CHandle::~CHandle()
			{
				Close();
			}

			void CHandle::Close()
			{
				if (IsValidHandle())
				{
					::CloseHandle(m_handle);
					m_handle = INVALID_HANDLE_VALUE;
				}
			}


			HANDLE& CHandle::GetHandle() 
			{ 
				return m_handle; 
			}

			const HANDLE& CHandle::GetHandle() const 
			{ 
				return m_handle; 
			}

            HANDLE& CHandle::GetHandleExc()
            {
                if(!IsValidHandle())
                    throw CExcBase("Uninitialize handle");

                return m_handle;
            }

            const HANDLE& CHandle::GetHandleExc() const
            {
                if(!IsValidHandle())
                    throw CExcBase("Uninitialize handle");

                return m_handle;
            }
 
			CHandle& CHandle::operator= (HANDLE handle)
			{
				Close();
				m_handle = handle;

				return *this;
			}

			bool CHandle::IsValidHandle() const
			{
				return m_handle != INVALID_HANDLE_VALUE && m_handle != NULL;
			}
		}
	}
}