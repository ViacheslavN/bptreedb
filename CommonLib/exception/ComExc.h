#pragma once

#include "exc_base.h"
#include <comdef.h>
namespace CommonLib
{
	class CComExc : public CExcBase
	{
	public:
		CComExc(HRESULT err);
		CComExc(_com_error& err);
		CComExc(const std::string& err_msg, HRESULT err);
		CComExc(const std::string& err_msg, _com_error& err);

		template<typename... Types>
		CComExc(HRESULT err, const std::string& format, Types&&... args) : CExcBase(format, std::forward<Types>(args)...), m_hr(err)
		{
			CExcBase::AddMsg(GetComErrorDesc(err));
		}

		~CComExc() noexcept;
		virtual std::shared_ptr<CExcBase> Clone() const;

		static std::string GetComErrorDesc(HRESULT err);
	private:

	private:
		HRESULT m_hr;

	};
}