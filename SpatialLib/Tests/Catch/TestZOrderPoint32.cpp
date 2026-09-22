#include "TestCommon.h"

using namespace bptreedb::spatiallib;
using namespace test_utils;

TEST_CASE("getShiftValue32 spreads 32 bits to the even positions", "[spatial][zorder][u32]")
{
	REQUIRE(getShiftValue32(0) == 0ull);
	REQUIRE(getShiftValue32(1) == 1ull);
	REQUIRE(getShiftValue32(3) == 5ull);
	REQUIRE(getShiftValue32(0xFFFFFFFFu) == 0x5555555555555555ull);
	REQUIRE(getShiftValue32(0x80000000u) == 0x4000000000000000ull);

	for (int i = 0; i < 50000; ++i)
	{
		const uint32_t v = RandomValue<uint32_t>();
		REQUIRE(getShiftValue32(v) == RefInterleave<uint64_t, uint32_t>(0, v));
	}
}

TEST_CASE("ZOrderPoint2DU32 constants and default state", "[spatial][zorder][u32]")
{
	ZOrderPoint2DU32 z;
	REQUIRE(z.m_nZValue == 0ull);
	REQUIRE(z.getBits() == 63);
	REQUIRE(ZOrderPoint2DU32::SizeInByte == 8);
	REQUIRE(sizeof(ZOrderPoint2DU32) == 8);
}

TEST_CASE("ZOrderPoint2DU32 interleaves and round-trips", "[spatial][zorder][u32]")
{
	REQUIRE(ZOrderPoint2DU32(uint32_t(0), uint32_t(1)).m_nZValue == 1ull);
	REQUIRE(ZOrderPoint2DU32(uint32_t(1), uint32_t(0)).m_nZValue == 2ull);
	REQUIRE(ZOrderPoint2DU32(0xFFFFFFFFu, 0u).m_nZValue == 0xAAAAAAAAAAAAAAAAull);
	REQUIRE(ZOrderPoint2DU32(0u, 0xFFFFFFFFu).m_nZValue == 0x5555555555555555ull);
	REQUIRE(ZOrderPoint2DU32(0xFFFFFFFFu, 0xFFFFFFFFu).m_nZValue == 0xFFFFFFFFFFFFFFFFull);

	SECTION("edge coordinates")
	{
		const uint32_t edges[] = { 0u, 1u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0xFFFFu, 0x10000u,
			0x00FFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFF000000u, 0xFFFFFFFEu, 0xFFFFFFFFu };
		for (uint32_t x : edges)
			for (uint32_t y : edges)
			{
				ZOrderPoint2DU32 z(x, y);
				REQUIRE(z.m_nZValue == RefInterleave<uint64_t, uint32_t>(x, y));
				uint32_t rx = 0, ry = 0;
				z.getXY(rx, ry);
				REQUIRE(rx == x);
				REQUIRE(ry == y);
			}
	}

	SECTION("random points")
	{
		for (int i = 0; i < 100000; ++i)
		{
			const uint32_t x = RandomValue<uint32_t>(), y = RandomValue<uint32_t>();
			ZOrderPoint2DU32 z(x, y);
			REQUIRE(z.m_nZValue == RefInterleave<uint64_t, uint32_t>(x, y));

			ZOrderPoint2DU32::TPoint p{ 0, 0 };
			z.getXY(p);
			REQUIRE(p.m_x == x);
			REQUIRE(p.m_y == y);
			REQUIRE(ZOrderPoint2DU32(p) == z);
		}
	}

	SECTION("random z values decode and re-encode to themselves")
	{
		for (int i = 0; i < 100000; ++i)
		{
			const uint64_t zv = RandomValue<uint64_t>();
			uint32_t x = 0, y = 0;
			ZOrderPoint2DU32(zv).getXY(x, y);
			REQUIRE(ZOrderPoint2DU32(x, y).m_nZValue == zv);
		}
	}
}

TEST_CASE("ZOrderPoint2DU32 assignment from a raw z value", "[spatial][zorder][u32]")
{
	ZOrderPoint2DU32 z;
	ZOrderPoint2DU32& ref = (z = uint64_t(0x5555555555555555ull));
	REQUIRE(&ref == &z);
	REQUIRE(z.m_nZValue == 0x5555555555555555ull);

	uint32_t x = 1, y = 0;
	z.getXY(x, y);
	REQUIRE(x == 0u);
	REQUIRE(y == 0xFFFFFFFFu); // top byte >= 0x80 decodes correctly
}

TEST_CASE("ZOrderPoint2DU32 comparison, sorting and arithmetic", "[spatial][zorder][u32]")
{
	ZOrderPoint2DU32 a(uint64_t(5)), b(uint64_t(1ull << 40)), a2(uint64_t(5));
	REQUIRE(a < b);
	REQUIRE(a <= a2);
	REQUIRE(b > a);
	REQUIRE(a == a2);
	REQUIRE_FALSE(a == b);

	REQUIRE((b - a).m_nZValue == (1ull << 40) - 5);
	REQUIRE((a + b).m_nZValue == (1ull << 40) + 5);
	ZOrderPoint2DU32 c = a;
	c += b;
	REQUIRE(c.m_nZValue == (1ull << 40) + 5);

	std::vector<ZOrderPoint2DU32> v;
	for (int i = 0; i < 1000; ++i)
		v.emplace_back(RandomValue<uint32_t>(), RandomValue<uint32_t>());
	std::sort(v.begin(), v.end(), ZPointComp<ZOrderPoint2DU32>());
	for (size_t i = 1; i < v.size(); ++i)
		REQUIRE(v[i - 1].m_nZValue <= v[i].m_nZValue);
}

TEST_CASE("ZOrderPoint2DU32 getBit exposes bit idx in its lowest bit", "[spatial][zorder][u32]")
{
	const ZOrderPoint2DU32 z(0xDEADBEEFu, 0x0BADF00Du);
	for (int idx = 0; idx <= z.getBits(); ++idx)
		REQUIRE((z.getBit(idx) & 1u) == ((z.m_nZValue >> idx) & 1u));
}

TEST_CASE("ZOrderPoint2DU32 setLowBits / clearLowBits", "[spatial][zorder][u32]")
{
	const uint64_t samples[] = { 0ull, ~0ull, 0xAAAAAAAAAAAAAAAAull, 0x5555555555555555ull,
		0x0123456789ABCDEFull, 0x8000000000000001ull };

	for (uint64_t zv : samples)
		for (int idx = 0; idx <= 63; ++idx)
		{
			ZOrderPoint2DU32 s(zv);
			s.setLowBits(idx);
			REQUIRE(s.m_nZValue == RefSetLowBits<uint64_t>(zv, idx));

			ZOrderPoint2DU32 c(zv);
			c.clearLowBits(idx);
			REQUIRE(c.m_nZValue == RefClearLowBits<uint64_t>(zv, idx));
		}
}

TEST_CASE("ZOrderPoint2DU32 IsInRect agrees with the decoded point", "[spatial][zorder][u32][rect]")
{
	const TRect2Du32 rect(1000000u, 2000000u, 3000000000u, 4000000000u);
	REQUIRE(ZOrderPoint2DU32(1000000u, 2000000u).IsInRect(rect));
	REQUIRE(ZOrderPoint2DU32(3000000000u, 4000000000u).IsInRect(rect));
	REQUIRE_FALSE(ZOrderPoint2DU32(999999u, 3000000u).IsInRect(rect));
	REQUIRE_FALSE(ZOrderPoint2DU32(2000000u, 4000000001u).IsInRect(rect));

	for (int i = 0; i < 20000; ++i)
	{
		const uint32_t x = RandomValue<uint32_t>(), y = RandomValue<uint32_t>();
		REQUIRE(ZOrderPoint2DU32(x, y).IsInRect(rect) == rect.isPoinInRect(x, y));
	}
}
