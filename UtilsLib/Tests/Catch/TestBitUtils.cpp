#include "TestCommon.h"
#include "UtilsLib/BitUtils.h"

using namespace bptreedb::utils;

namespace
{
	// Reference floor(log2(v)) for v > 0
	int32_t RefLog2(uint64_t v)
	{
		int32_t r = -1;
		while (v) { v >>= 1; ++r; }
		return r;
	}
}

TEST_CASE("log2 uint64_t", "[bitutils][log2]")
{
	SECTION("powers of two")
	{
		for (int32_t i = 0; i < 64; ++i)
			REQUIRE(log2(uint64_t(1) << i) == i);
	}

	SECTION("powers of two minus one")
	{
		for (int32_t i = 1; i < 64; ++i)
			REQUIRE(log2((uint64_t(1) << i) - 1) == i - 1);
	}

	SECTION("max value")
	{
		REQUIRE(log2(uint64_t(-1)) == 63);
	}

	SECTION("assorted values against reference")
	{
		const uint64_t values[] = { 1, 2, 3, 5, 100, 1000, 65535, 65536, 0x123456789ull, 0xFFFFFFFFull, 0x100000000ull, 0x7FFFFFFFFFFFFFFFull };
		for (uint64_t v : values)
			REQUIRE(log2(v) == RefLog2(v));
	}

	SECTION("int64_t overload")
	{
		REQUIRE(log2(int64_t(1)) == 0);
		REQUIRE(log2(int64_t(1024)) == 10);
		REQUIRE(log2(int64_t(1) << 40) == 40);
	}
}

TEST_CASE("log2 uint32_t", "[bitutils][log2]")
{
	SECTION("powers of two")
	{
		for (int32_t i = 0; i < 32; ++i)
			REQUIRE(log2(uint32_t(1) << i) == i);
	}

	SECTION("powers of two minus one")
	{
		for (int32_t i = 1; i < 32; ++i)
			REQUIRE(log2((uint32_t(1) << i) - 1) == i - 1);
	}

	SECTION("max value")
	{
		REQUIRE(log2(uint32_t(-1)) == 31);
	}

	SECTION("exhaustive small range against reference")
	{
		for (uint32_t v = 1; v < 70000; ++v)
			REQUIRE(log2(v) == RefLog2(v));
	}

	SECTION("int32_t overload")
	{
		REQUIRE(log2(int32_t(1)) == 0);
		REQUIRE(log2(int32_t(7)) == 2);
		REQUIRE(log2(int32_t(8)) == 3);
		REQUIRE(log2(int32_t(0x7FFFFFFF)) == 30);
	}
}

TEST_CASE("log2 uint16_t", "[bitutils][log2]")
{
	SECTION("exhaustive against reference")
	{
		for (uint32_t v = 1; v <= 0xFFFF; ++v)
			REQUIRE(log2(uint16_t(v)) == RefLog2(v));
	}

	SECTION("int16_t overload")
	{
		REQUIRE(log2(int16_t(1)) == 0);
		REQUIRE(log2(int16_t(256)) == 8);
		REQUIRE(log2(int16_t(0x7FFF)) == 14);
	}
}
