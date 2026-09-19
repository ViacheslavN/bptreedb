#pragma once
namespace CommonLib
{
	namespace system
	{
		namespace win
		{

			typedef std::shared_ptr<class CHandle> CHandlePtr;

			class CHandle
			{
			public:
				CHandle(HANDLE handle = INVALID_HANDLE_VALUE);
				~CHandle();

				operator HANDLE() const { return m_handle; }
				operator HANDLE() { return m_handle; }

				CHandle& operator= (HANDLE handle);

				HANDLE& GetHandle();
				const HANDLE& GetHandle() const;

                HANDLE& GetHandleExc();
                const HANDLE& GetHandleExc() const;

				CHandle(const CHandle& handle) = delete;
				CHandle& operator= (const CHandle& handle) = delete;

				bool  IsValidHandle() const;
                void Close();
			private:
				HANDLE m_handle;
			};

	
		}
	}
}