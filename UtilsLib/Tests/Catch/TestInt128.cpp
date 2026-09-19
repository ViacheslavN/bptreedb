#include "TestCommon.h"
#include "UtilsLib/Int128.h"

using bptreedb::util::CUInt128;

namespace
{
	CUInt128 Make(uint64_t hi, uint64_t lo)
	{
		CUInt128 v;
		v.m_hi = hi;
		v.m_lo = lo;
		return v;
	}
}

TEST_CASE("CUInt128 is zero-initialised", "[int128]")
{
	CUInt128 v;
	REQUIRE(v.m_hi == 0);
	REQUIRE(v.m_lo == 0);
}

TEST_CASE("CUInt128 equality", "[int128]")
{
	REQUIRE(Make(1, 2) == Make(1, 2));
	REQUIRE_FALSE(Make(1, 2) != Make(1, 2));
	REQUIRE(Make(1, 2) != Make(1, 3));
	REQUIRE(Make(1, 2) != Make(2, 2));
	REQUIRE(Make(0, 0) == CUInt128());
}

TEST_CASE("CUInt128 ordering - high word dominates", "[int128]")
{
	CUInt128 a = Make(0, uint64_t(-1)); // 2^64 - 1
	CUInt128 b = Make(1, 0);            // 2^64

	REQUIRE(a < b);
	REQUIRE(a <= b);
	REQUIRE_FALSE(b < a);
	REQUIRE(b >= a);
	REQUIRE_FALSE(a >= b);
}

TEST_CASE("CUInt128 ordering - same high word compares low word", "[int128]")
{
	CUInt128 a = Make(7, 10);
	CUInt128 b = Make(7, 11);

	REQUIRE(a < b);
	REQUIRE(a <= b);
	REQUIRE(b >= a);
	REQUIRE_FALSE(b <= a);
	REQUIRE_FALSE(a >= b);
}

TEST_CASE("CUInt128 ordering - equal values", "[int128]")
{
	CUInt128 a = Make(3, 4);
	CUInt128 b = Make(3, 4);

	REQUIRE_FALSE(a < b);
	REQUIRE(a <= b);
	REQUIRE(a >= b);
}

TEST_CASE("CUInt128 ordering is consistent with std::sort", "[int128]")
{
	std::vector<CUInt128> v = { Make(2, 0), Make(0, 5), Make(1, uint64_t(-1)), Make(0, 1), Make(1, 0) };
	std::sort(v.begin(), v.end());

	REQUIRE(v[0] == Make(0, 1));
	REQUIRE(v[1] == Make(0, 5));
	REQUIRE(v[2] == Make(1, 0));
	REQUIRE(v[3] == Make(1, uint64_t(-1)));
	REQUIRE(v[4] == Make(2, 0));
}

TEST_CASE("CUInt128 Add without overflow", "[int128]")
{
	CUInt128 v = Make(5, 10);
	v.Add(32);
	REQUIRE(v.m_hi == 5);
	REQUIRE(v.m_lo == 42);
}

TEST_CASE("CUInt128 Add carries into the high word", "[int128][!mayfail]")
{
	// Add() has a "TO DO" on carry handling; this documents the intended semantics.
	CUInt128 v = Make(0, uint64_t(-1));
	v.Add(1);
	CHECK(v.m_lo == 0);
	CHECK(v.m_hi == 1);
}

TEST_CASE("CUInt128 Save / Load round trip", "[int128][stream]")
{
	CommonLib::IAllocPtr pAlloc = CommonLib::IAlloc::CreateSimpleAlloc();

	const CUInt128 values[] = {
		Make(0, 0),
		Make(0, 1),
		Make(1, 0),
		Make(0x0123456789ABCDEFull, 0xFEDCBA9876543210ull),
		Make(uint64_t(-1), uint64_t(-1)),
	};

	for (const CUInt128& src : values)
	{
		auto pWrite = std::make_shared<CommonLib::CWriteMemoryStream>(pAlloc);
		CUInt128 tmp = src;
		tmp.Save(pWrite);
		REQUIRE(pWrite->Pos() == 2 * sizeof(uint64_t));

		auto pRead = std::make_shared<CommonLib::CReadMemoryStream>();
		pRead->AttachBuffer(pWrite->Buffer(), pWrite->Pos());

		CUInt128 dst;
		dst.Load(pRead);
		REQUIRE(dst == src);
		REQUIRE(pRead->Pos() == 2 * sizeof(uint64_t));
	}
}

TEST_CASE("CUInt128 several values in one stream", "[int128][stream]")
{
	CommonLib::IAllocPtr pAlloc = CommonLib::IAlloc::CreateSimpleAlloc();
	auto pWrite = std::make_shared<CommonLib::CWriteMemoryStream>(pAlloc);

	const int kCount = 16;
	for (int i = 0; i < kCount; ++i)
		Make(i, i * 1000).Save(pWrite);

	auto pRead = std::make_shared<CommonLib::CReadMemoryStream>();
	pRead->AttachBuffer(pWrite->Buffer(), pWrite->Pos());

	for (int i = 0; i < kCount; ++i)
	{
		CUInt128 v;
		v.Load(pRead);
		REQUIRE(v == Make(i, i * 1000));
	}
}
