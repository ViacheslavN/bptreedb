#include "stdafx.h"
#include "ComExc.h"
#include "../str/StringEncoding.h"


namespace CommonLib
{
	CComExc::CComExc(HRESULT err) : m_hr(err)
	{
		CExcBase::AddMsg(GetComErrorDesc(err));
	}

	CComExc::CComExc(_com_error& err) : m_hr(err.Error())
	{
		if (err.Error() != S_OK)
		{
            std::wstring errDesc;
			bool descriptionIsNull = !err.Description();
			if (!descriptionIsNull)
				errDesc = err.Description();

			CExcBase::AddMsg(str_format::AStrFormatSafeT("com err: {0}, decs: {1}", StringEncoding::str_w2utf8_safe(err.ErrorMessage()), StringEncoding::str_w2utf8_safe(errDesc)));
			CExcBase::AddMsg(GetComErrorDesc(m_hr));
		}
	}

	CComExc::CComExc(const std::string& err_msg, _com_error& err) : CExcBase(err_msg), m_hr(err.Error())
	{
		if (err.Error() != S_OK)
		{
            std::wstring errDesc;
			bool descriptionIsNull = !err.Description();
			if (!descriptionIsNull)
				errDesc = err.Description();

			CExcBase::AddMsg(str_format::AStrFormatSafeT("com err: {0}, decs: {1}", StringEncoding::str_w2utf8_safe(err.ErrorMessage()), StringEncoding::str_w2utf8_safe(errDesc)));
			CExcBase::AddMsg(GetComErrorDesc(m_hr));
		}
	}

	CComExc::CComExc(const std::string& err_msg, HRESULT err) : CExcBase(err_msg), m_hr(err)
	{
		CExcBase::AddMsg(GetComErrorDesc(m_hr));
	}
	
	CComExc::~CComExc() noexcept
	{

	}

	std::shared_ptr<CExcBase> CComExc::Clone() const
	{
		std::shared_ptr<CComExc> ptrExc(new CComExc(*this));
		return ptrExc;
	}

    std::string CComExc::GetComErrorDesc(HRESULT hr)
	{
		_com_error err(hr);
		return StringEncoding::str_w2utf8_safe(err.ErrorMessage());
	}
}