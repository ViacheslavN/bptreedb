#pragma once
#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/guid/guid.h"

namespace bptreedb
{
	enum eTransactionDataType
	{
		eTT_UNDEFINED = 1,
		eTT_MODIFY = 2,
		eTT_SELECT = 4,
		eTT_DDL = 8
	};

	enum eTransationType
	{
		eTT2PL,
		eWAL
	};

	class ITransaction
	{
	public:
		ITransaction() {};
		virtual ~ITransaction() {}

		virtual void Commit() = 0;
		virtual void Rollback() = 0;

	};
} 
