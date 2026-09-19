#pragma once

#include "SqliteApi.h"

namespace CommonLib
{
	namespace database
	{
		namespace sqlite
		{

			class CTransaction : public ITransaction
			{
			public:
				CTransaction(CSQliteApiPtr ptrDB, bool isReadOnly);
				~CTransaction();

				virtual void Begin();
				virtual void Commit();
				virtual void Rollback();
			private:
				void RollbackImpl();

				CSQliteApiPtr m_ptrDB;
				bool m_IsReadOnly;
				bool m_start{ false };
			};

		}
		
	}
}