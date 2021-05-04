#include "pch.h"
#include "../../BPTrees/BaseDIffEncoder.h"
#include "../../BPTrees/Compressor/EntropyEncoder/SignedNumLenEncoder.h"
#include "../../BPTrees/AllocsSet.h"

typedef bptreedb::TValueDiffEncoder<int64_t, int64_t, bptreedb::CSignedNumLenEncoder> TEncoder;

typedef CommonLib::STLAllocator<int64_t> TAlloc;
typedef std::vector<int64_t, TAlloc> TValueMemSet;

typedef TDataGenerator<int64_t, TRandomInegerGenerator<int64_t> > TTestDataGenerator;
typedef std::shared_ptr<TTestDataGenerator> TTestDataGeneratorPtr;


void TestSignedEncoderImpl(const TValueMemSet& vecInData)
{
	bptreedb::TCompressorParamsBasePtr ptrCompParmas(new bptreedb::CompressorParamsBase());
	bptreedb::TCompressorParamsPtr  ptrCompInnerKey(new bptreedb::CompressorParams());

	ptrCompParmas->AddCompressParams(ptrCompInnerKey, bptreedb::eInnerKey);


	CommonLib::IAllocPtr  ptrAlloc = CommonLib::IAlloc::CreateSimpleAlloc();
	bptreedb::TAllocsSetPtr ptrAllocSet(new bptreedb::CAllocsSet(ptrAlloc));
	TAlloc vecAlloc(ptrAlloc);
	TValueMemSet vecData(vecAlloc);
	TEncoder encoder(ptrAllocSet, ptrCompParmas, bptreedb::eInnerKey);
	bptreedb::CBPTreeContext context(ptrAlloc);
	uint32_t encodeSize = 0;
	for (int64_t i = 0; i < vecInData.size(); ++i)
	{
		vecData.push_back(vecInData[i]);
		encoder.AddSymbol(vecData.size(), i, vecInData[i], vecData);
		encodeSize = encoder.GetCompressSize();
		if (encodeSize > 8192)
			break;
	}

	CommonLib::CWriteMemoryStream stream;
	stream.Resize(100000);
	memset(stream.Buffer(), 0, stream.Size());

	encoder.BeginEncoding(vecData);
	uint32_t size = encoder.Encode(vecData, &stream, encodeSize, &context);

	encoder.Clear();

	CommonLib::CReadMemoryStream readStream;
	readStream.AttachBuffer(stream.Buffer(), stream.Pos());

	TValueMemSet vecData1(vecAlloc);
	encoder.Decode(vecData.size(), vecData1, &readStream, stream.Pos(), &context);

	for (size_t i = 0; i < vecData.size(); ++i)
	{
		if (vecData[i] != vecData1[i])
		{
			std::cout << "vecData[i] != vecData1[i] i: " << i << ", vecData[i]: " << vecData[i] << ", vecData1[i]: " << vecData1[i] << "\n";
		}
	}
}


void TestOneSign()
{
	TAlloc alloc;
	TValueMemSet vecInData(alloc);
	for (int64_t i = 0; i < 100000; ++i)
	{
		vecInData.push_back(i);
	}
	
	TestSignedEncoderImpl(vecInData);
}

void TestRandomSign()
{
	TAlloc alloc;
	TValueMemSet vecInData(alloc);
	wstr dataPath = L"F:\\BPMapTestData.data";
	uint32_t nCount = 10000;

	TTestDataGeneratorPtr pDataGenerator(new TTestDataGenerator((uint32_t)nCount, dataPath));
	pDataGenerator->Open();

	for (uint32_t i = 0; i < nCount; ++i)
	{
		vecInData.push_back(pDataGenerator->GetValue(i));
	}

	TestSignedEncoderImpl(vecInData);
}


void TestSignedEncoder()
{
	TestRandomSign();
	//TestOneSign();
}