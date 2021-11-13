// MixTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "BitStream.h"

void TestStringCompressor();
void TestEntropyEncoder();
void TestSignedEncoder();


int main()
{

	bptreedb::CVariant variant = 123.4;
	bptreedb::CVariant strVariant = astr("123445");
	bptreedb::CVariant str1Variant = strVariant;
	TestSignedEncoder();
	return 0;

	std::vector<byte_t> vecbitsBuffer;

	vecbitsBuffer.resize(10000, 0);

	CBitWriter writer;
	CBitReader reader;

	writer.AttachBuffer(vecbitsBuffer.data(), vecbitsBuffer.size());
	reader.AttachBuffer(vecbitsBuffer.data(), vecbitsBuffer.size());

	for (uint64_t i = 0xFFFFFFFFFFFFFFFE -300; i < 0xFFFFFFFFFFFFFFFF ; ++i)
	{
		uint32_t nBitLen = bptreedb::utils::log2(i);
		writer.WriteBits(i, nBitLen);
	}

	for (uint64_t i = 0xFFFFFFFFFFFFFFFE - 300; i < 0xFFFFFFFFFFFFFFFF; ++i)
	{
		uint32_t nBitLen = bptreedb::utils::log2(i);
		uint64_t dd = 0;
		reader.ReadBits(dd, nBitLen);
		dd |= ((uint64_t)1 << (nBitLen ));
		if (dd != i)
			std::cout << "not match\n";
	}



	uint32_t bit_pos = 30;
	uint32_t dd = ~1;
	unsigned int byte_offset = bit_pos >> 3;
	byte_offset &= ~1;

	uint16_t shift = (bit_pos & 0xf);

	TestEntropyEncoder();

	return 0;

	uint32_t nVal = 2439055;
	uint32_t nBitLen = bptreedb::utils::log2(nVal);

	uint32_t nVal1 = 502345;
	uint32_t nBitLen1 = bptreedb::utils::log2(nVal1);

	//nBitLen += 1;
	std::vector<byte_t> buf(100,0);

	CommonLib::CFxBitWriteStream bitWStream;
	bitWStream.AttachBuffer(buf.data(), buf.size());
	bitWStream.WriteBits(nVal, nBitLen);
	bitWStream.WriteBits(nVal1, nBitLen1);

	CommonLib::CFxBitReadStream bitRStream;

	bitRStream.AttachBuffer(buf.data(), buf.size());
	uint32_t nVal1_1;
	uint32_t nVal1_2;
	bitRStream.ReadBits(nVal1_1, nBitLen);
	nVal1_1 |= ((uint32_t)1 << (nBitLen));

	bitRStream.ReadBits(nVal1_2, nBitLen1);
	nVal1_2 |= ((uint32_t)1 << (nBitLen1));

	//TestStringCompressor();
	return 0;


	typedef bptreedb::utils::TCacheLRU_2Q<int, int> TCache;
	TCache m_cache(std::shared_ptr<CommonLib::IAlloc>(), -1);

	for (int i = 0; i < 10; ++i)
	{
		m_cache.AddElem(i, i);
	}

	CommonLib::CPerfCounter counter(10);

	counter.StartOperation(1);
	for (int i = 0; i < 100000000; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			int d = m_cache.GetElem(j);
			if (d == -1)
				std::cout << "error";
		}
	}
	counter.StopOperation(1);
	std::cout << "Time: " << counter.GetTimeOperation(1) << "\n";
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
