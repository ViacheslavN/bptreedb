#pragma once


#include "../exception/exc_base.h"
#include "log.h"

namespace CommonLib
{

	class CLogger
	{
	public:
				
		virtual  void MessageStr(const std::string& msg);
		
		CLogger();
		virtual  ~CLogger();

		void InnitLog(std::shared_ptr<IlogLineFormat> pLogLineFormat,  std::shared_ptr<IlogWriter> pLogWriter);

		virtual void Msg(eMsgType type, const std::string& msg);
		virtual void Msg(eMsgType type, const std::string& format, const std::string& msg);
		virtual void Msg(eMsgType type, const std::string& format, const std::string& msg, const std::string& msg1);
		virtual void Msg(eMsgType type, const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2);
		virtual void Msg(eMsgType type, const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4);
		virtual void Msg(eMsgType type, const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4, const std::string& msg5);
		virtual void Msg(eMsgType type, const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4, const std::string& msg5, const std::string& msg6);
		
		template<class TArg>
		void MsgT(eMsgType type, TArg& arg)
		{
			Msg(type, str_utils::AStrFrom(arg));
		}

		template<class TArg>
		void MsgT(eMsgType type, const std::string& format, const TArg& arg)
		{
			Msg(type, format, str_utils::AStrFrom(arg));
		}

		template<class TArg, class TArg1>
		void MsgT(eMsgType type, const std::string& format, const TArg& arg, const TArg1& arg1)
		{
			Msg(type, format, str_utils::AStrFrom(arg), str_utils::AStrFrom(arg1));
		}

		template<class TArg1, class TArg2, class TArg3>
		void MsgT(eMsgType type, const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
		{
			Msg(type, format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3));
		}

		template<class TArg1, class TArg2, class TArg3, class TArg4>
		void MsgT(eMsgType type, const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
		{
			Msg(type, format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4));
		}


		virtual void Trace(const std::string& msg);
		virtual void Trace(const std::string& format, const std::string& msg);
		virtual void Trace(const std::string& format, const std::string& msg, const std::string& msg1);
		virtual void Trace(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2);
		virtual void Trace(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4);
		virtual void Trace(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4, const std::string& msg5);
		virtual void Trace(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4, const std::string& msg5, const std::string& msg6);

		template<class TArg>
		void TraceT(TArg& arg)
		{
			Trace(str_utils::AStrFrom(arg));
		}

		template<class TArg>
		void TraceT(const std::string& format, const TArg& arg)
		{
			Trace(format, str_utils::AStrFrom(arg));
		}

		template<class TArg, class TArg1>
		void TraceT(const std::string& format, const TArg& arg, const TArg1& arg1)
		{
			Trace(format, str_utils::AStrFrom(arg), str_utils::AStrFrom(arg1));
		}

		template<class TArg1, class TArg2, class TArg3>
		void TraceT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
		{
			Trace(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3));
		}

		template<class TArg1, class TArg2, class TArg3, class TArg4>
		void TraceT( const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
		{
			Trace(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4));
		}
			   

		virtual void Info(const std::string& msg);
		virtual void Info(const std::string& format, const std::string& msg);
		virtual void Info(const std::string& format, const std::string& msg, const std::string& msg1);
		virtual void Info(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2);
		virtual void Info(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4);
		virtual void Info(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4, const std::string& msg5);
		virtual void Info(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4, const std::string& msg5, const std::string& msg6);


		template<class TArg>
		void InfoT(TArg& arg)
		{
			Info(str_utils::AStrFrom(arg));
		}

		template<class TArg>
		void InfoT(const std::string& format, const TArg& arg)
		{
			Info(format, str_utils::AStrFrom(arg));
		}

		template<class TArg, class TArg1>
		void InfoT(const std::string& format, const TArg& arg, const TArg1& arg1)
		{
			Info(format, str_utils::AStrFrom(arg), str_utils::AStrFrom(arg1));
		}

		template<class TArg1, class TArg2, class TArg3>
		void InfoT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
		{
			Info(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3));
		}

		template<class TArg1, class TArg2, class TArg3, class TArg4>
		void InfoT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
		{
			Info(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4));
		}
		
		virtual void Error(const std::string& msg);
		virtual void Error(const std::string& format, const std::string& msg);
		virtual void Error(const std::string& format, const std::string& msg, const std::string& msg1);
		virtual void Error(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2);
		virtual void Error(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4);
		virtual void Error(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4, const std::string& msg5);
		virtual void Error(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg4, const std::string& msg5, const std::string& msg6);

		template<class TArg>
		void ErrorT(TArg& arg)
		{
			Error(str_utils::AStrFrom(arg));
		}

		template<class TArg>
		void ErrorT(const std::string& format, const TArg& arg)
		{
			Error(format, str_utils::AStrFrom(arg));
		}

		template<class TArg, class TArg1>
		void ErrorT(const std::string& format, const TArg& arg, const TArg1& arg1)
		{
			Error(format, str_utils::AStrFrom(arg), str_utils::AStrFrom(arg1));
		}

		template<class TArg1, class TArg2, class TArg3>
		void ErrorT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
		{
			Error(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3));
		}

		template<class TArg1, class TArg2, class TArg3, class TArg4>
		void ErrorT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
		{
			Error(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4));
		}
			   		 

		virtual void Warning(const std::string& msg);
		virtual void Warning(const std::string& format, const std::string& msg);
		virtual void Warning(const std::string& format, const std::string& msg, const std::string& msg1);
		virtual void Warning(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2);
		virtual void Warning(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg3);
		virtual void Warning(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg3, const std::string& msg4);
		virtual void Warning(const std::string& format, const std::string& msg, const std::string& msg1, const std::string& msg2, const std::string& msg3, const std::string& msg4, const std::string& msg5);

		template<class TArg>
		void WarningT(TArg& arg)
		{
			Warning(str_utils::AStrFrom(arg));
		}

		template<class TArg>
		void WarningT(const std::string& format, const TArg& arg)
		{
			Warning(format, str_utils::AStrFrom(arg));
		}

		template<class TArg, class TArg1>
		void WarningT(const std::string& format, const TArg& arg, const TArg1& arg1)
		{
			Warning(format, str_utils::AStrFrom(arg), str_utils::AStrFrom(arg1));
		}

		template<class TArg1, class TArg2, class TArg3>
		void WarningT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3)
		{
			Warning(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3));
		}

		template<class TArg1, class TArg2, class TArg3, class TArg4>
		void WarningT(const std::string& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4)
		{
			Warning(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4));
		}
					   		 

		virtual void Exc(const std::exception& exc);
 

		/*
		template<class TArg>
		void ExcT(std::exception& exc)
		{
			Exc(exc);
		}
		
		template<class TArg>
		void ExcT(const astr& format, std::exception& exc)
		{
			Exc(format, exc);
		}

		template<class TArg>
		void ExcT(const astr& format, const TArg& arg, std::exception& exc)
		{
			Exc(format, str_utils::AStrFrom(arg), exc);
		}

		template<class TArg, class TArg1>
		void ExcT(const astr& format, const TArg& arg, const TArg1& arg1, std::exception& exc)
		{
			Exc(format, str_utils::AStrFrom(arg), str_utils::AStrFrom(arg1), exc);
		}

		template<class TArg1, class TArg2, class TArg3>
		void ExcT(const astr& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, std::exception& exc)
		{
			Exc(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), exc);
		}

		template<class TArg1, class TArg2, class TArg3, class TArg4>
		void ExcT(const astr& format, const TArg1& arg1, const TArg2& arg2, const TArg3& arg3, const TArg4& arg4, std::exception& exc)
		{
			Exc(format, str_utils::AStrFrom(arg1), str_utils::AStrFrom(arg2), str_utils::AStrFrom(arg3), str_utils::AStrFrom(arg4), exc);
		}*/
		protected:
			void ChangeCurrentLogFile();

		protected:
			int m_logLevel;
				
			std::shared_ptr<IlogLineFormat> m_pLogLineFormat;
			std::shared_ptr<IlogWriter> m_pLogWriter;
	};

}


extern CommonLib::CLogger Log;