#pragma once
#include <exception>
#include "../synch/thread.h"
#include "../str/StringEncoding.h"
#include "../str/str.h"

namespace CommonLib
{

class CExcBase : public std::exception
{
public:
	CExcBase() noexcept;
	CExcBase(const CExcBase& value) noexcept;
	CExcBase(const std::string& msg) noexcept;
	CExcBase(const std::wstring& msg) noexcept;

	template<class TArg>
	CExcBase(const std::string& format, const TArg& arg) : m_srcThreadId(synch::CThread::GetCurThreadId())
	{
		AddMsgT(format, arg);
	}

	template<class TArg1, class TArg2>
	CExcBase(const std::string& format, const TArg1& arg1, const TArg2& arg2) : m_srcThreadId(synch::CThread::GetCurThreadId())
	{
		AddMsgT(format, arg1, arg2);
	}

	template<class TArg1, class TArg2, class TArg3>
	CExcBase(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3) : m_srcThreadId(synch::CThread::GetCurThreadId())
	{
		AddMsgT(format, arg1, arg2, arg3);
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	CExcBase(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4) : m_srcThreadId(synch::CThread::GetCurThreadId())
	{
		AddMsgT(format, arg1, arg2, arg3, arg4);
	}
	

	template<class TArg>
	CExcBase(const std::wstring& format, const TArg& arg) : m_srcThreadId(synch::CThread::GetCurThreadId())
	{
		AddMsgT(format, arg);
	}

	template<class TArg1, class TArg2>
	CExcBase(const std::wstring& format, const TArg1& arg1, const TArg2& arg2) : m_srcThreadId(synch::CThread::GetCurThreadId())
	{
		AddMsgT(format, arg1, arg2);
	}

	template<class TArg1, class TArg2, class TArg3>
	CExcBase(const std::wstring& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3) : m_srcThreadId(synch::CThread::GetCurThreadId())
	{
		AddMsgT(format, arg1, arg2, arg3);
	}


	virtual ~CExcBase() noexcept;

	virtual std::shared_ptr<CExcBase> Clone() const;
	virtual const char*  what() const noexcept;
	virtual void Throw() const;
	
	const std::vector<std::string >& GetChain() const;

	virtual void AddMsg(const std::string& msg);
	virtual void AddMsg(const std::string& format, const std::string& msg);
	virtual void AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2);
	virtual void AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2, const std::string& msg3);
	virtual void AddMsg(const std::string& format, const std::string& msg1, const std::string& msg2, const std::string& msg3, const std::string& msg4);
	   
	template<class TArg>
	void AddMsgT(const std::string& format, const TArg& arg1)
	{
		return AddMsg(format, str_utils::AStrFrom(arg1));
	}

	template<class TArg1, class TArg2>
	void AddMsgT(const std::string& format, const TArg1& arg1, const TArg2& arg2)
	{
		return AddMsg(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2));
	}

	template<class TArg1, class TArg2, class TArg3>
	void AddMsgT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
	{
		return AddMsg(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	void AddMsgT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
	{
		return AddMsg(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4));
	}




	template<class TArg>
	void AddMsgT(const std::wstring& format, const TArg& arg1)
	{
		return AddMsg(StringEncoding::str_w2utf8_safe(format), str_utils::AStrFrom(arg1));
	}

	template<class TArg1, class TArg2>
	void AddMsgT(const std::wstring& format, const TArg1& arg1, const TArg2& arg2)
	{
		return AddMsg(StringEncoding::str_w2utf8_safe(format), str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2));
	}

	template<class TArg1, class TArg2, class TArg3>
	void AddMsgT(const std::wstring& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
	{
		return AddMsg(StringEncoding::str_w2utf8_safe(format), str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3));
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	void AddMsgT(const std::wstring& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
	{
		return AddMsg(StringEncoding::str_w2utf8_safe(format), str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4));
	}


	virtual void AddMsg(const std::wstring& msg);
	virtual void AddMsg(const std::wstring& format, const std::wstring& msg);
	virtual void AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2);
	virtual void AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2, const std::wstring& msg3);
	virtual void AddMsg(const std::wstring& format, const std::wstring& msg1, const std::wstring& msg2, const std::wstring& msg3, const std::wstring& msg4);

	template<class TArg>
	static void RegenExcT( const TArg& arg, std::exception& exc_src)
	{
		return RegenExc(str_utils::AStrFrom(arg), exc_src);
	}
	
	template<class TArg>
	static void RegenExcT(const std::string& format, const TArg& arg1, std::exception& exc_src)
	{
		return RegenExc(format, str_utils::AStrFrom(arg1), exc_src);
	}

	template<class TArg1, class TArg2>
	static void RegenExcT(const std::string& format, const TArg1& arg1, const TArg2& arg2, std::exception& exc_src)
	{
		return RegenExc(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), exc_src);
	}

	template<class TArg1, class TArg2, class TArg3>
	static void RegenExcT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, std::exception& exc_src)
	{
		return RegenExc(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), exc_src);
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	static void RegenExcT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4, std::exception& exc_src)
	{
		return RegenExc(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4), exc_src);
	}

	static void RegenExc(const std::string& msg, std::exception& exc_src);
	static void RegenExc(const std::string& format, const std::string& msg, std::exception& exc_src);
	static void RegenExc(const std::string& format, const std::string& msg1, const std::string& msg2, std::exception& exc_src);
	static void RegenExc(const std::string& format, const std::string& msg1, const std::string& msg2, const std::string& msg3, std::exception& exc_src);
	static void RegenExc(const std::string& format, const std::string& msg1, const std::string& msg2, const std::string& msg3, const std::string& msg4, std::exception& exc_src);
	static std::string GetErrorDesc(const std::exception& exc);
	static std::shared_ptr<CExcBase> CloneFromExc(const std::exception& exc);
	static std::vector<std::string > GetChainFromExc(const std::exception& exc);
protected:
    std::vector<std::string > m_msgChain;
	thread_id_t m_srcThreadId;
	mutable std::string m_what;

};

}