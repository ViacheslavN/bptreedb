// TestSQlite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <malloc.h>
#include "../../CommonLib.h"

#include "../../sqlitelib/Database.h"
#include "../../crypto/EmptyDataCipher.h"
#include "../../str/str.h"
#include "../../sqlitelib/CryptoContext.h"
#include "../../crypto/winapi/CryptoFactory.h"
#include "../../crypto/openssllib/CryptoFactoryOpenSSL.h"


class CryptoContext : public CommonLib::database::ICryptoContext
{
public:
	CryptoContext() {}
	virtual ~CryptoContext() {}

	virtual CommonLib::crypto::IDataCipherPtr GetDataCipher()
	{
		std::shared_ptr<CommonLib::crypto::IDataCipher> ptrDataCipher(new CommonLib::crypto::CEmptyDataCipher());
		return ptrDataCipher;
	}

	virtual CommonLib::crypto::IXTSDataCipherPtr GetXTSDataCipher()
	{
		std::shared_ptr<CommonLib::crypto::IXTSDataCipher> ptrDataCipher(new CommonLib::crypto::CEmptyXTSDataCipher());
		return ptrDataCipher;
	}

	virtual size_t GetInitBlockSize() const
	{
		return 152000;
	}

	virtual void CreateInitBlock(byte_t *pBuf, size_t size)
	{

	}

	virtual bool ValidateInitBlock(byte_t *pBuf, size_t size)
	{
		return true;
	}
};




void TestDatabase(CommonLib::crypto::ICryptoFactoryPtr ptrCryptoFactory, const std::string& strPath, const std::string& pwd)
{
 
	std::shared_ptr<CommonLib::database::ICryptoContext> ptrCryptoContext(new CommonLib::database::CCryptoContext(pwd.c_str(), CommonLib::database::PwdKey, ptrCryptoFactory));
	CommonLib::database::ICryptoContext::RemoveCryptoContext(strPath);
	CommonLib::database::ICryptoContext::AddCryptoContext(strPath, ptrCryptoContext);

	CommonLib::database::IDatabasePtr ptrDatabase = CommonLib::database::IDatabaseSQLiteCreator::Create(strPath.c_str(), uint32_t(CommonLib::database::CreateDatabase | CommonLib::database::WAL));
	ptrDatabase->SetBusyTimeout(1000);


	if (ptrDatabase->IsTableExists("testTable"))
	{
		{
			CommonLib::database::IStatmentPtr ptrStatment = ptrDatabase->PrepareQuery("SELECT contact_id, first_name, last_name, email, phone from testTable");
			while (ptrStatment->Next())
			{
				std::cout << ptrStatment->ReadInt32(1) << "\n";
				std::cout << ptrStatment->ReadText(2) << "\n";
				std::cout << ptrStatment->ReadText(3) << "\n";
				std::cout << ptrStatment->ReadText(4) << "\n";
				std::cout << ptrStatment->ReadText(5) << "\n";
			}
		}

		CommonLib::database::ITransactionPtr ptrTran = ptrDatabase->CreateTransaction();
		ptrTran->Begin();
		ptrDatabase->Execute("drop table testTable");
		ptrTran->Commit();
	}

	{

		CommonLib::database::ITransactionPtr ptrTran = ptrDatabase->CreateTransaction();
		ptrTran->Begin();
		ptrDatabase->Execute("CREATE TABLE testTable ( "
			" contact_id INTEGER PRIMARY KEY, "
			" first_name TEXT NOT NULL, "
			" last_name TEXT NOT NULL, "
			" email TEXT NOT NULL, "
			" phone TEXT NOT NULL) ");
		ptrTran->Commit();
	}

	int key = 0;
	for (int t = 0; t < 100; ++t)
	{
		CommonLib::database::ITransactionPtr ptrTran = ptrDatabase->CreateTransaction();
		ptrTran->Begin();

		CommonLib::database::IStatmentPtr ptrStatment = ptrDatabase->PrepareQuery("INSERT INTO testTable(contact_id, first_name, last_name, email, phone) VALUES(?,?,?,?,?)");
		for (int i = 0; i < 100; ++i)
		{			
			ptrStatment->Reset();
			ptrStatment->BindInt32(1, key);
			ptrStatment->BindText(2, CommonLib::str_format::AStrFormatSafeT("first_name_{0}", key), true);
			ptrStatment->BindText(3, CommonLib::str_format::AStrFormatSafeT("last_name_{0}", key), true);
			ptrStatment->BindText(4, CommonLib::str_format::AStrFormatSafeT("email_{0}", key), true);
			ptrStatment->BindText(5, CommonLib::str_format::AStrFormatSafeT("phone_{0}", key), true);

			key += 1;
			ptrStatment->Next();
		}

		ptrTran->Commit();
	}
}


int main()
{
	try
	{
		CommonLib::crypto::ICryptoFactoryPtr ptrCryptoFactory(new CommonLib::crypto::winapi::CCryptoFactory());
		CommonLib::crypto::ICryptoFactoryPtr ptrCryptoFactorySSL(new CommonLib::crypto::openssllib::CCryptoFactoryOpenSSL());
        std::string dbPath = "F:\\test.db";
        std::string pwd = "12334ddffdfdfg";

		//TestDatabase(ptrCryptoFactorySSL, dbPath, pwd);
		TestDatabase(ptrCryptoFactory, dbPath, pwd);
		TestDatabase(ptrCryptoFactorySSL, dbPath, pwd);
		
	}
	catch (std::exception& exc )
	{
		std::cout << "Error: " << exc.what();
	}

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
