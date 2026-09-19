#pragma once

#include "exc_base.h"

namespace CommonLib
{
	class CWinNtExc : public CExcBase
	{
	public:
		CWinNtExc(NTSTATUS ntStatus);
		CWinNtExc(const std::string& err_msg, NTSTATUS ntStatus);

		template<typename... Types>
		CWinNtExc(NTSTATUS ntStatus, const std::string& format, Types&&... args) : CExcBase(format, std::forward<Types>(args)...), m_ntStatus(ntStatus)
		{
			CExcBase::AddMsg(GetErrorDesc(ntStatus));
		}

		virtual ~CWinNtExc() noexcept;
		virtual std::shared_ptr<CExcBase> Clone() const;

		static std::string GetErrorDesc(NTSTATUS ntStatus);
	private:

	private:
		NTSTATUS m_ntStatus;

	};
}