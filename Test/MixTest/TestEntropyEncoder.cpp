#include "pch.h"

#include "TestEncoder.h"

typedef TDataGenerator<int64_t, TRandomInegerGenerator<int64_t> > TTestDataGenerator;
typedef std::shared_ptr<TTestDataGenerator> TTestDataGeneratorPtr;



void SetNumLenEncoder(std::vector<uint64_t>& vecData, testencoding::TestEncoder& encoder)
{
	for (uint32_t i = 1; i < vecData.size(); ++i)
	{
		encoder.AddSymbol(vecData[i] - vecData[i - 1]);
	}
}

void TestEncoder(CommonLib::TPrefCounterPtr pPerf, std::vector<uint64_t>& vecData, testencoding::TestEncoder& encoder, CommonLib::IWriteStream* pStream)
{

	CommonLib::CPrefCounterHolder holder(pPerf, testencoding::TestEncoder::Operations::eEncoding);
	encoder.BeginEncoding(pStream);
	pStream->Write(vecData[0]);
	for (size_t i = 1; i < vecData.size(); ++i)
	{
		encoder.EncodeSymbol(vecData[i] - vecData[i - 1]);
	}

	encoder.FinishEncoding(pStream);
}

void TestEntropyEncoder()
{


	std::vector<uint64_t> vecData;
	wstr dataPath = L"F:\\BPMapTestData.data";
	uint32_t nCount = 737;

	TTestDataGeneratorPtr pDataGenerator(new TTestDataGenerator((uint32_t)800, dataPath));
	pDataGenerator->Open();

	for (uint32_t i = 0; i < nCount; ++i)
	{
		vecData.push_back(pDataGenerator->GetValue(i));
	}

	std::sort(vecData.begin(), vecData.end());

	CommonLib::TPrefCounterPtr pPerf(new CommonLib::CPerfCounter(20));
	testencoding::TestEncoder encoder(pPerf);

	SetNumLenEncoder(vecData, encoder);

	uint32_t size = encoder.GetCompressSize();

	std::vector<byte_t> vec;
	vec.resize(20000);

	CommonLib::CFxMemoryWriteStream stream;
	stream.AttachBuffer(vec.data(), vec.size(), false);
	

	std::cout << "Start encoding test \n";

	uint32_t nStepCount = 300000;
	int64_t nStep = nStepCount / 100;

	for (size_t i = 0; i < nStepCount; ++i)
	{
		stream.Seek(0, CommonLib::soFromBegin);
		memset(vec.data(), 0, vec.size());

		TestEncoder(pPerf, vecData, encoder, &stream);

		if (i%nStep == 0)
		{
			std::cout << i << "  " << (i * 100) / nStepCount << " %" << '\r';
		}
	}

	std::cout << "Encoding time: " << pPerf->GetTimeOperation(testencoding::TestEncoder::Operations::eEncoding) <<", coun: " << pPerf->GetCountOperation(testencoding::TestEncoder::Operations::eEncoding) << "\n";
	std::cout << "range encoding: " << pPerf->GetTimeOperation(testencoding::TestEncoder::Operations::eRangeEncode) << ", count: " << pPerf->GetCountOperation(testencoding::TestEncoder::Operations::eRangeEncode) << "\n";
	std::cout << "bits writing: " << pPerf->GetTimeOperation(testencoding::TestEncoder::Operations::eBitsWrite) << ", count: " << pPerf->GetCountOperation(testencoding::TestEncoder::Operations::eBitsWrite) << "\n";




}