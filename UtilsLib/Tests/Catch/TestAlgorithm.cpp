#include "TestCommon.h"
#include "UtilsLib/algorithm.h"

using namespace bptreedb::utils;

TEST_CASE("lower_bound on empty array returns -1", "[algorithm][lower_bound]")
{
	const int32_t* pData = nullptr;
	REQUIRE(lower_bound<int32_t>(pData, 0, 5) == -1);
}

TEST_CASE("lower_bound matches std::lower_bound", "[algorithm][lower_bound]")
{
	std::vector<int32_t> data = { 1, 3, 3, 5, 7, 9, 9, 9, 12 };
	const uint32_t nSize = (uint32_t)data.size();

	SECTION("key present (first occurrence)")
	{
		REQUIRE(lower_bound(data.data(), nSize, 3) == 1);
		REQUIRE(lower_bound(data.data(), nSize, 9) == 5);
		REQUIRE(lower_bound(data.data(), nSize, 1) == 0);
		REQUIRE(lower_bound(data.data(), nSize, 12) == 8);
	}

	SECTION("key absent - insertion point")
	{
		REQUIRE(lower_bound(data.data(), nSize, 0) == 0);
		REQUIRE(lower_bound(data.data(), nSize, 4) == 3);
		REQUIRE(lower_bound(data.data(), nSize, 10) == 8);
	}

	SECTION("key greater than all elements returns size")
	{
		REQUIRE(lower_bound(data.data(), nSize, 100) == (int32_t)nSize);
	}

	SECTION("exhaustive comparison against std::lower_bound")
	{
		for (int32_t key = -1; key <= 14; ++key)
		{
			auto it = std::lower_bound(data.begin(), data.end(), key);
			REQUIRE(lower_bound(data.data(), nSize, key) == (int32_t)(it - data.begin()));
		}
	}
}

TEST_CASE("upper_bound on empty array returns -1", "[algorithm][upper_bound]")
{
	const int32_t* pData = nullptr;
	REQUIRE(upper_bound<int32_t>(pData, 0, 5) == -1);
}

TEST_CASE("upper_bound matches std::upper_bound", "[algorithm][upper_bound]")
{
	std::vector<int32_t> data = { 1, 3, 3, 5, 7, 9, 9, 9, 12 };
	const uint32_t nSize = (uint32_t)data.size();

	SECTION("key present - index after last occurrence")
	{
		REQUIRE(upper_bound(data.data(), nSize, 3) == 3);
		REQUIRE(upper_bound(data.data(), nSize, 9) == 8);
		REQUIRE(upper_bound(data.data(), nSize, 12) == (int32_t)nSize);
	}

	SECTION("key absent")
	{
		REQUIRE(upper_bound(data.data(), nSize, 0) == 0);
		REQUIRE(upper_bound(data.data(), nSize, 4) == 3);
		REQUIRE(upper_bound(data.data(), nSize, 100) == (int32_t)nSize);
	}

	SECTION("exhaustive comparison against std::upper_bound")
	{
		for (int32_t key = -1; key <= 14; ++key)
		{
			auto it = std::upper_bound(data.begin(), data.end(), key);
			REQUIRE(upper_bound(data.data(), nSize, key) == (int32_t)(it - data.begin()));
		}
	}
}

TEST_CASE("binary_search", "[algorithm][binary_search]")
{
	std::vector<int64_t> data = { -10, -3, 0, 4, 8, 15, 16, 23, 42 };
	const uint32_t nSize = (uint32_t)data.size();

	SECTION("empty array returns -1")
	{
		REQUIRE(binary_search<int64_t>(nullptr, 0, 4) == -1);
	}

	SECTION("every element is found at its own index")
	{
		for (uint32_t i = 0; i < nSize; ++i)
			REQUIRE(binary_search(data.data(), nSize, data[i]) == (int32_t)i);
	}

	SECTION("missing keys return -1")
	{
		REQUIRE(binary_search<int64_t>(data.data(), nSize, -11) == -1);
		REQUIRE(binary_search<int64_t>(data.data(), nSize, 1) == -1);
		REQUIRE(binary_search<int64_t>(data.data(), nSize, 43) == -1);
	}

	SECTION("single element")
	{
		int64_t one = 7;
		REQUIRE(binary_search<int64_t>(&one, 1, 7) == 0);
		REQUIRE(binary_search<int64_t>(&one, 1, 6) == -1);
		REQUIRE(binary_search<int64_t>(&one, 1, 8) == -1);
	}

	SECTION("duplicates - returns first occurrence")
	{
		std::vector<int64_t> dup = { 1, 2, 2, 2, 3 };
		REQUIRE(binary_search<int64_t>(dup.data(), (uint32_t)dup.size(), 2) == 1);
	}
}

TEST_CASE("algorithms work with non-integral types", "[algorithm]")
{
	std::vector<std::string> data = { "apple", "banana", "cherry", "date" };
	const uint32_t nSize = (uint32_t)data.size();

	REQUIRE(lower_bound(data.data(), nSize, std::string("banana")) == 1);
	REQUIRE(upper_bound(data.data(), nSize, std::string("banana")) == 2);
	REQUIRE(binary_search(data.data(), nSize, std::string("cherry")) == 2);
	REQUIRE(binary_search(data.data(), nSize, std::string("coconut")) == -1);
}
