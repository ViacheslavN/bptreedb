#include "pch.h"
#include "../../BPTrees/Compressor/StringCompressor/ZLibStringEncoder.h"



void GetRandomValues(byte *pData, uint32_t nSize)
{
	if (!nSize)
		return;


	for (uint32_t  i = 0; i < nSize; ++i)
	{
		uint32_t val = (rand() % 10 + 1);
		 
		if(val > 9)
			pData[i] = byte_t(val) + 'a';
		else
			pData[i] = byte_t(val) + '0';
	}

}

void TestStringCompressor()
{

	bptreedb::TCompressorParamsBasePtr pCompParmas(new bptreedb::CompressorParamsBase());
	bptreedb::TCompressorParamsPtr pCompInnerKey(new bptreedb::CompressorParams());


	CommonLib::IAllocPtr pAlloc = CommonLib::IAlloc::CreateSimpleAlloc();

	bptreedb::CBPTreeContext context(pAlloc);


 	pCompInnerKey->SetIntParam("compressLevel", 9);
	pCompInnerKey->SetIntParam("compressRate", 10);

	pCompParmas->AddCompressParams(pCompInnerKey, bptreedb::eInnerKey);



	bptreedb::CZlibStringComp comp(pAlloc, pCompParmas, bptreedb::eInnerKey);
	typedef CommonLib::STLAllocator<bptreedb::StringValue> TAlloc;
	typedef std::vector<bptreedb::StringValue, TAlloc> TValueMemSet;

	TValueMemSet valueSet;

	char rndstr[25];
	memset(rndstr, 0, sizeof(rndstr));

	srand(time(0));

	uint32_t nCount = 1000;

	for (uint32_t i = 0; i < nCount; ++i)
	{

		astr str = CommonLib::str_utils::AStrInt32(i);

		GetRandomValues((byte *)rndstr, sizeof(rndstr));

		rndstr[sizeof(rndstr) - 1] = '\0';

		str += rndstr;

		astr utf8 = CommonLib::StringEncoding::str_a2utf8(str);

		bptreedb::StringValue value;
		value.m_nLen = utf8.length();
		value.m_utf8 = (byte_t*)pAlloc->Alloc(utf8.length());
		memcpy(value.m_utf8, utf8.c_str(), utf8.length());

		valueSet.push_back(value);

		comp.AddSymbol(i, i, value, valueSet);

	}


	byte_t page[8192];
	CommonLib::CFxMemoryWriteStream writeStream;
	while (true)
	{
	
		writeStream.AttachBuffer(page, sizeof(page));
		if(comp.Encode(valueSet, &writeStream, sizeof(page), &context) == 0)
			break;


		for (size_t i = valueSet.size() / 2; i < valueSet.size(); ++i)
		{
			comp.RemoveSymbol(valueSet.size(), i, valueSet[i], valueSet);
		}

		valueSet.resize(valueSet.size() / 2);
	
	}


	CommonLib::CReadMemoryStream readStream;
	readStream.AttachBuffer(writeStream.Buffer(), writeStream.Pos());

	TValueMemSet valueDecSet;

	bptreedb::CZlibStringComp decomp(pAlloc, pCompParmas, bptreedb::eInnerKey);

	decomp.Decode(valueSet.size(), valueDecSet, &readStream, writeStream.Pos(), &context);

	for (size_t i = 0; i < valueDecSet.size(); ++i)
	{
		if (strcmp((const char*)valueDecSet[i].m_utf8, (const char*)valueSet[i].m_utf8) != 0)
			std::cout << L"error string";

	}



}


