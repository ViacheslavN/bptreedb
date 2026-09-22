#include "TestCommon.h"

using namespace bptreedb::spatiallib;
using namespace test_utils;

// ---------------------------------------------------------------------------
// Bit spreading helpers
// ---------------------------------------------------------------------------

TEST_CASE("getShiftValue16 spreads 16 bits to the even positions", "[spatial][zorder][u16]")
{
	REQUIRE(getShiftValue16(0) == 0u);
	REQUIRE(getShiftValue16(1) == 1u);
	REQUIRE(getShiftValue16(2) == 4u);
	REQUIRE(getShiftValue16(3) == 5u);
	REQUIRE(getShiftValue16(0xFFFF) == 0x55555555u);
	REQUIRE(getShiftValue16(0x8000) == 0x40000000u);

	for (uint32_t v = 0; v <= 0xFFFF; ++v)
		REQUIRE(getShiftValue16(uint16_t(v)) == RefInterleave<uint32_t, uint16_t>(0, uint16_t(v)));
}

// ---------------------------------------------------------------------------
// ZOrderPoint2DU16
// ---------------------------------------------------------------------------

TEST_CASE("ZOrderPoint2DU16 constants and default state", "[spatial][zorder][u16]")
{
	ZOrderPoint2DU16 z;
	REQUIRE(z.m_nZValue == 0u);
	REQUIRE(z.getBits() == 31);
	REQUIRE(ZOrderPoint2DU16::SizeInByte == 4);
	REQUIRE(sizeof(ZOrderPoint2DU16) == 4);

	uint16_t x = 1, y = 1;
	z.getXY(x, y);
	REQUIRE(x == 0);
	REQUIRE(y == 0);
}

TEST_CASE("ZOrderPoint2DU16 interleaves x into odd and y into even bits", "[spatial][zorder][u16]")
{
	REQUIRE(ZOrderPoint2DU16(uint16_t(0), uint16_t(0)).m_nZValue == 0u);
	REQUIRE(ZOrderPoint2DU16(uint16_t(0), uint16_t(1)).m_nZValue == 1u); // y -> bit 0
	REQUIRE(ZOrderPoint2DU16(uint16_t(1), uint16_t(0)).m_nZValue == 2u); // x -> bit 1
	REQUIRE(ZOrderPoint2DU16(uint16_t(1), uint16_t(1)).m_nZValue == 3u);
	REQUIRE(ZOrderPoint2DU16(uint16_t(0xFFFF), uint16_t(0)).m_nZValue == 0xAAAAAAAAu);
	REQUIRE(ZOrderPoint2DU16(uint16_t(0), uint16_t(0xFFFF)).m_nZValue == 0x55555555u);
	REQUIRE(ZOrderPoint2DU16(uint16_t(0xFFFF), uint16_t(0xFFFF)).m_nZValue == 0xFFFFFFFFu);

	for (int i = 0; i < 20000; ++i)
	{
		const uint16_t x = RandomValue<uint16_t>(), y = RandomValue<uint16_t>();
		REQUIRE(ZOrderPoint2DU16(x, y).m_nZValue == RefInterleave<uint32_t, uint16_t>(x, y));
	}
}

TEST_CASE("ZOrderPoint2DU16 getXY inverts setZOrder", "[spatial][zorder][u16]")
{
	SECTION("every x with fixed y values, and every y with fixed x values")
	{
		const uint16_t fixed[] = { 0, 1, 0x00FF, 0x0100, 0x7FFF, 0x8000, 0xAAAA, 0xFFFF };
		for (uint32_t v = 0; v <= 0xFFFF; ++v)
		{
			for (uint16_t f : fixed)
			{
				uint16_t x = 0, y = 0;
				ZOrderPoint2DU16(uint16_t(v), f).getXY(x, y);
				REQUIRE(x == v);
				REQUIRE(y == f);

				ZOrderPoint2DU16(f, uint16_t(v)).getXY(x, y);
				REQUIRE(x == f);
				REQUIRE(y == v);
			}
		}
	}

	SECTION("random points")
	{
		for (int i = 0; i < 100000; ++i)
		{
			const uint16_t x = RandomValue<uint16_t>(), y = RandomValue<uint16_t>();
			uint16_t rx = 0, ry = 0;
			ZOrderPoint2DU16(x, y).getXY(rx, ry);
			REQUIRE(rx == x);
			REQUIRE(ry == y);
		}
	}

	SECTION("every 32-bit z value decodes to a point that encodes back to it (sampled)")
	{
		for (uint64_t zv = 0; zv <= 0xFFFFFFFFull; zv += 65521) // prime stride over the whole range
		{
			const ZOrderPoint2DU16 z{ uint32_t(zv) };
			uint16_t x = 0, y = 0;
			z.getXY(x, y);
			REQUIRE(ZOrderPoint2DU16(x, y).m_nZValue == uint32_t(zv));
		}
	}
}

TEST_CASE("ZOrderPoint2DU16 TPoint overloads", "[spatial][zorder][u16]")
{
	const ZOrderPoint2DU16::TPoint p{ 1234, 54321 };
	ZOrderPoint2DU16 z(p);
	REQUIRE(z == ZOrderPoint2DU16(uint16_t(1234), uint16_t(54321)));

	ZOrderPoint2DU16::TPoint back{ 0, 0 };
	z.getXY(back);
	REQUIRE(back == p);
}

TEST_CASE("ZOrderPoint2DU16 comparison follows the z value", "[spatial][zorder][u16]")
{
	ZOrderPoint2DU16 a(uint32_t(10)), b(uint32_t(20)), a2(uint32_t(10));

	REQUIRE(a < b);
	REQUIRE(a <= b);
	REQUIRE(a <= a2);
	REQUIRE(b > a);
	REQUIRE(a == a2);
	REQUIRE_FALSE(a == b);
	REQUIRE_FALSE(b < a);
	REQUIRE_FALSE(a > a2);

	SECTION("Morton property: an aligned 2^k x 2^k block occupies z values [0, 4^k)")
	{
		for (int k = 1; k <= 6; ++k)
		{
			const uint32_t side = 1u << k;
			std::vector<uint32_t> zs;
			for (uint32_t x = 0; x < side; ++x)
				for (uint32_t y = 0; y < side; ++y)
					zs.push_back(ZOrderPoint2DU16(uint16_t(x), uint16_t(y)).m_nZValue);
			std::sort(zs.begin(), zs.end());
			for (uint32_t i = 0; i < zs.size(); ++i)
				REQUIRE(zs[i] == i); // a perfect permutation of 0 .. 4^k-1
		}
	}

	SECTION("ZPointComp sorts like operator<")
	{
		std::vector<ZOrderPoint2DU16> v;
		for (int i = 0; i < 1000; ++i)
			v.emplace_back(RandomValue<uint16_t>(), RandomValue<uint16_t>());
		std::vector<ZOrderPoint2DU16> w = v;

		std::sort(v.begin(), v.end(), ZPointComp<ZOrderPoint2DU16>());
		std::sort(w.begin(), w.end());
		REQUIRE(v == w);

		ZPointComp<ZOrderPoint2DU16> comp;
		for (size_t i = 1; i < v.size(); ++i)
		{
			REQUIRE_FALSE(comp.LE(v[i], v[i - 1]));
			REQUIRE(comp.EQ(v[i], v[i]));
		}
	}
}

TEST_CASE("ZOrderPoint2DU16 assignment from a raw z value", "[spatial][zorder][u16]")
{
	ZOrderPoint2DU16 z(uint16_t(1), uint16_t(2));
	ZOrderPoint2DU16& ref = (z = uint32_t(0xAAAAAAAA));
	REQUIRE(&ref == &z); // returns *this, so it chains like a normal assignment
	REQUIRE(z.m_nZValue == 0xAAAAAAAAu);

	uint16_t x = 0, y = 1;
	z.getXY(x, y);
	REQUIRE(x == 0xFFFF);
	REQUIRE(y == 0);

	ZOrderPoint2DU16 a, b;
	a = b = uint32_t(7);
	REQUIRE(a.m_nZValue == 7u);
	REQUIRE(b.m_nZValue == 7u);
}

TEST_CASE("ZOrderPoint2DU16 arithmetic works on the raw z value", "[spatial][zorder][u16]")
{
	ZOrderPoint2DU16 a(uint32_t(100)), b(uint32_t(30));
	REQUIRE((a - b).m_nZValue == 70u);
	REQUIRE((a + b).m_nZValue == 130u);
	a += b;
	REQUIRE(a.m_nZValue == 130u);

	// wraps like uint32_t
	REQUIRE((ZOrderPoint2DU16(uint32_t(0)) - ZOrderPoint2DU16(uint32_t(1))).m_nZValue == 0xFFFFFFFFu);
	REQUIRE((ZOrderPoint2DU16(uint32_t(0xFFFFFFFF)) + ZOrderPoint2DU16(uint32_t(1))).m_nZValue == 0u);
}

TEST_CASE("ZOrderPoint2DU16 getBit exposes bit idx in its lowest bit", "[spatial][zorder][u16]")
{
	const ZOrderPoint2DU16 z(uint16_t(0xA5C3), uint16_t(0x3C5A));
	for (int idx = 0; idx <= z.getBits(); ++idx)
	{
		ZOrderPoint2DU16 copy = z; // getBit is non-const on U16
		REQUIRE((copy.getBit(idx) & 1u) == ((z.m_nZValue >> idx) & 1u));
	}
}

TEST_CASE("ZOrderPoint2DU16 setLowBits / clearLowBits", "[spatial][zorder][u16]")
{
	const uint32_t samples[] = { 0u, 0xFFFFFFFFu, 0xAAAAAAAAu, 0x55555555u, 0x12345678u, 0x80000001u };

	for (uint32_t zv : samples)
	{
		for (int idx = 0; idx <= 31; ++idx)
		{
			ZOrderPoint2DU16 s(zv);
			s.setLowBits(idx);
			REQUIRE(s.m_nZValue == RefSetLowBits<uint32_t>(zv, idx));

			ZOrderPoint2DU16 c(zv);
			c.clearLowBits(idx);
			REQUIRE(c.m_nZValue == RefClearLowBits<uint32_t>(zv, idx));
		}
	}

	SECTION("only the dimension of idx is touched")
	{
		// idx 9 is an x bit (odd): y bits must survive both operations
		ZOrderPoint2DU16 z(uint16_t(0), uint16_t(0xFFFF));
		z.setLowBits(9);
		uint16_t x = 0, y = 0;
		z.getXY(x, y);
		REQUIRE(y == 0xFFFF);
		REQUIRE(x == 0x000F); // x bits 0..3 set, bit 4 (== z bit 9) cleared

		z.clearLowBits(9);
		z.getXY(x, y);
		REQUIRE(y == 0xFFFF);
		REQUIRE(x == 0x0010); // x bit 4 set, x bits 0..3 cleared
	}
}

TEST_CASE("ZOrderPoint2DU16 IsInRect agrees with the decoded point", "[spatial][zorder][u16][rect]")
{
	const TRect2Du16 rect(100, 200, 300, 400);

	REQUIRE(ZOrderPoint2DU16(uint16_t(100), uint16_t(200)).IsInRect(rect));
	REQUIRE(ZOrderPoint2DU16(uint16_t(300), uint16_t(400)).IsInRect(rect));
	REQUIRE(ZOrderPoint2DU16(uint16_t(150), uint16_t(399)).IsInRect(rect));
	REQUIRE_FALSE(ZOrderPoint2DU16(uint16_t(99), uint16_t(300)).IsInRect(rect));
	REQUIRE_FALSE(ZOrderPoint2DU16(uint16_t(200), uint16_t(401)).IsInRect(rect));

	std::mt19937 rng(3);
	std::uniform_int_distribution<int> dist(0, 500);
	for (int i = 0; i < 10000; ++i)
	{
		const uint16_t x = uint16_t(dist(rng)), y = uint16_t(dist(rng));
		REQUIRE(ZOrderPoint2DU16(x, y).IsInRect(rect) == rect.isPoinInRect(x, y));
	}
}
