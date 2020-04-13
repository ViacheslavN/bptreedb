// MixTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int main()
{

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
