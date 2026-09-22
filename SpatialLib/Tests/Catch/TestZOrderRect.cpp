#include "TestCommon.h"

// The rect Z-order keys treat a rect as a point in 4D (xMin, yMin, xMax, yMax) and
// interleave the four coordinates, four bits per "level".

using namespace bptreedb::spatiallib;
using namespace test_utils;

namespace
{
	struct SCoords { uint64_t xMin, yMin, xMax, yMax; };

	// A spread of interesting coordinate values for the given width.
	std::vector<uint64_t> EdgeValues(unsigned bits)
	{
		std::vector<uint64_t> v = { 0, 1, 2, 3, 7, 8, 0xFF, 0x100, 0x5555, 0xAAAA };
		const uint64_t top = bits == 64 ? ~0ull : ((static_cast<uint64_t>(1) << bits) - 1);
		v.push_back(top);
		v.push_back(top - 1);
		v.push_back(top / 2);
		v.push_back(top / 2 + 1);
		for (uint64_t& x : v)
			x &= top;
		return v;
	}
}

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

TEST_CASE("Rect Z-order constants", "[spatial][zrect]")
{
	REQUIRE(ZOrderRect2DU16::SizeInByte == 8);
	REQUIRE(ZOrderRect2DU32::SizeInByte == 16);
	REQUIRE(ZOrderRect2DU64::SizeInByte == 32);

	REQUIRE(ZOrderRect2DU16().getBits() == 63);
	REQUIRE(ZOrderRect2DU32().getBits() == 127);
	REQUIRE(ZOrderRect2DU64().getBits() == 255);

	REQUIRE(ZOrderRect2DU16::coordMax == 0xFFFFu);
	REQUIRE(ZOrderRect2DU32::coordMax == 0xFFFFFFFFu);
	REQUIRE(ZOrderRect2DU64::coordMax == ~0ull);

	// four coordinates of N bits produce a 4N-bit key
	REQUIRE(sizeof(ZOrderRect2DU16) == 8);
	REQUIRE(sizeof(ZOrderRect2DU32) == 16);
	REQUIRE(sizeof(ZOrderRect2DU64) == 32);
}

TEST_CASE("Rect Z-order default state is all zero", "[spatial][zrect]")
{
	REQUIRE(ZOrderRect2DU16().m_nZValue == 0ull);

	ZOrderRect2DU32 z32;
	REQUIRE(z32.m_nZValue[0] == 0ull);
	REQUIRE(z32.m_nZValue[1] == 0ull);

	ZOrderRect2DU64 z64;
	for (int i = 0; i < 4; ++i)
		REQUIRE(z64.m_nZValue[i] == 0ull);
}

// ---------------------------------------------------------------------------
// Encoding
// ---------------------------------------------------------------------------

TEST_CASE("ZOrderRect2DU16 interleaves the four coordinates", "[spatial][zrect][u16]")
{
	SECTION("one bit per dimension")
	{
		// lowest bit of each coordinate lands in the lowest nibble
		REQUIRE(ZOrderRect2DU16(0, 0, 0, 1).m_nZValue == 0x1ull); // yMax -> bit 0
		REQUIRE(ZOrderRect2DU16(0, 1, 0, 0).m_nZValue == 0x2ull); // yMin -> bit 1
		REQUIRE(ZOrderRect2DU16(0, 0, 1, 0).m_nZValue == 0x4ull); // xMax -> bit 2
		REQUIRE(ZOrderRect2DU16(1, 0, 0, 0).m_nZValue == 0x8ull); // xMin -> bit 3
		REQUIRE(ZOrderRect2DU16(1, 1, 1, 1).m_nZValue == 0xFull);
	}

	SECTION("a full coordinate spreads over every fourth bit")
	{
		REQUIRE(ZOrderRect2DU16(0, 0, 0, 0xFFFF).m_nZValue == 0x1111111111111111ull);
		REQUIRE(ZOrderRect2DU16(0, 0xFFFF, 0, 0).m_nZValue == 0x2222222222222222ull);
		REQUIRE(ZOrderRect2DU16(0, 0, 0xFFFF, 0).m_nZValue == 0x4444444444444444ull);
		REQUIRE(ZOrderRect2DU16(0xFFFF, 0, 0, 0).m_nZValue == 0x8888888888888888ull);
		REQUIRE(ZOrderRect2DU16(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF).m_nZValue == ~0ull);
	}

	SECTION("against the reference interleave")
	{
		for (uint64_t xMin : EdgeValues(16))
			for (uint64_t yMin : EdgeValues(16))
			{
				const ZOrderRect2DU16 z(static_cast<uint16_t>(xMin), static_cast<uint16_t>(yMin), static_cast<uint16_t>(xMin), static_cast<uint16_t>(yMin));
				uint64_t ref = 0;
				RefInterleaveRect<uint16_t>(static_cast<uint16_t>(xMin), static_cast<uint16_t>(yMin), static_cast<uint16_t>(xMin), static_cast<uint16_t>(yMin), &ref, 1);
				REQUIRE(z.m_nZValue == ref);
			}

		for (int i = 0; i < 20000; ++i)
		{
			const uint16_t a = RandomValue<uint16_t>(), b = RandomValue<uint16_t>();
			const uint16_t c = RandomValue<uint16_t>(), d = RandomValue<uint16_t>();
			uint64_t ref = 0;
			RefInterleaveRect<uint16_t>(a, b, c, d, &ref, 1);
			REQUIRE(ZOrderRect2DU16(a, b, c, d).m_nZValue == ref);
		}
	}
}

TEST_CASE("ZOrderRect2DU32 and U64 interleave the four coordinates", "[spatial][zrect][u32][u64]")
{
	SECTION("U32 against the reference")
	{
		for (int i = 0; i < 20000; ++i)
		{
			const uint32_t a = RandomValue<uint32_t>(), b = RandomValue<uint32_t>();
			const uint32_t c = RandomValue<uint32_t>(), d = RandomValue<uint32_t>();
			uint64_t ref[2] = { 0, 0 };
			RefInterleaveRect<uint32_t>(a, b, c, d, ref, 2);

			const ZOrderRect2DU32 z(a, b, c, d);
			REQUIRE(z.m_nZValue[0] == ref[0]);
			REQUIRE(z.m_nZValue[1] == ref[1]);
		}
	}

	SECTION("U64 against the reference")
	{
		for (int i = 0; i < 20000; ++i)
		{
			const uint64_t a = RandomValue<uint64_t>(), b = RandomValue<uint64_t>();
			const uint64_t c = RandomValue<uint64_t>(), d = RandomValue<uint64_t>();
			uint64_t ref[4] = { 0, 0, 0, 0 };
			RefInterleaveRect<uint64_t>(a, b, c, d, ref, 4);

			const ZOrderRect2DU64 z(a, b, c, d);
			for (int w = 0; w < 4; ++w)
				REQUIRE(z.m_nZValue[w] == ref[w]);
		}
	}
}

// ---------------------------------------------------------------------------
// Round trips
// ---------------------------------------------------------------------------

TEST_CASE("ZOrderRect2DU16 getXY inverts setZOrder", "[spatial][zrect][u16]")
{
	SECTION("edge coordinates")
	{
		for (uint64_t a : EdgeValues(16))
			for (uint64_t b : EdgeValues(16))
			{
				const ZOrderRect2DU16 z(static_cast<uint16_t>(a), static_cast<uint16_t>(b), static_cast<uint16_t>(b), static_cast<uint16_t>(a));
				uint16_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				z.getXY(xMin, yMin, xMax, yMax);
				REQUIRE(xMin == static_cast<uint16_t>(a));
				REQUIRE(yMin == static_cast<uint16_t>(b));
				REQUIRE(xMax == static_cast<uint16_t>(b));
				REQUIRE(yMax == static_cast<uint16_t>(a));
			}
	}

	SECTION("random rects, and through the TRect overloads")
	{
		for (int i = 0; i < 50000; ++i)
		{
			uint16_t xMin = RandomValue<uint16_t>(), xMax = RandomValue<uint16_t>();
			uint16_t yMin = RandomValue<uint16_t>(), yMax = RandomValue<uint16_t>();
			if (xMin > xMax) std::swap(xMin, xMax);
			if (yMin > yMax) std::swap(yMin, yMax);

			const TRect2Du16 rect(xMin, yMin, xMax, yMax);
			const ZOrderRect2DU16 z(rect);
			REQUIRE(z == ZOrderRect2DU16(xMin, yMin, xMax, yMax));

			TRect2Du16 back;
			z.getXY(back);
			REQUIRE(back == rect);
		}
	}

	SECTION("every z value decodes to a rect that encodes back to it (sampled)")
	{
		for (uint64_t step = 0; step < 50000; ++step)
		{
			const uint64_t zv = step * 0x9E3779B97F4A7C15ull; // spread over the whole range
			const ZOrderRect2DU16 z(zv);
			uint16_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
			z.getXY(xMin, yMin, xMax, yMax);
			REQUIRE(ZOrderRect2DU16(xMin, yMin, xMax, yMax).m_nZValue == zv);
		}
	}
}

TEST_CASE("ZOrderRect2DU32 getXY inverts setZOrder", "[spatial][zrect][u32]")
{
	for (uint64_t a : EdgeValues(32))
		for (uint64_t b : EdgeValues(32))
		{
			const ZOrderRect2DU32 z(static_cast<uint32_t>(a), static_cast<uint32_t>(b), static_cast<uint32_t>(b), static_cast<uint32_t>(a));
			uint32_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
			z.getXY(xMin, yMin, xMax, yMax);
			REQUIRE(xMin == static_cast<uint32_t>(a));
			REQUIRE(yMin == static_cast<uint32_t>(b));
			REQUIRE(xMax == static_cast<uint32_t>(b));
			REQUIRE(yMax == static_cast<uint32_t>(a));
		}

	for (int i = 0; i < 50000; ++i)
	{
		uint32_t xMin = RandomValue<uint32_t>(), xMax = RandomValue<uint32_t>();
		uint32_t yMin = RandomValue<uint32_t>(), yMax = RandomValue<uint32_t>();
		if (xMin > xMax) std::swap(xMin, xMax);
		if (yMin > yMax) std::swap(yMin, yMax);

		const TRect2Du32 rect(xMin, yMin, xMax, yMax);
		TRect2Du32 back;
		ZOrderRect2DU32(rect).getXY(back);
		REQUIRE(back == rect);
	}
}

TEST_CASE("ZOrderRect2DU64 getXY inverts setZOrder", "[spatial][zrect][u64]")
{
	for (uint64_t a : EdgeValues(64))
		for (uint64_t b : EdgeValues(64))
		{
			const ZOrderRect2DU64 z(a, b, b, a);
			uint64_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
			z.getXY(xMin, yMin, xMax, yMax);
			REQUIRE(xMin == a);
			REQUIRE(yMin == b);
			REQUIRE(xMax == b);
			REQUIRE(yMax == a);
		}

	for (int i = 0; i < 50000; ++i)
	{
		uint64_t xMin = RandomValue<uint64_t>(), xMax = RandomValue<uint64_t>();
		uint64_t yMin = RandomValue<uint64_t>(), yMax = RandomValue<uint64_t>();
		if (xMin > xMax) std::swap(xMin, xMax);
		if (yMin > yMax) std::swap(yMin, yMax);

		const TRect2Du64 rect(xMin, yMin, xMax, yMax);
		TRect2Du64 back;
		ZOrderRect2DU64(rect).getXY(back);
		REQUIRE(back == rect);
	}
}

// ---------------------------------------------------------------------------
// Ordering
// ---------------------------------------------------------------------------

TEST_CASE("ZOrderRect2DU16 ordering follows the z value", "[spatial][zrect][u16]")
{
	const ZOrderRect2DU16 a(static_cast<uint64_t>(10)), b(static_cast<uint64_t>(20)), a2(static_cast<uint64_t>(10));
	REQUIRE(a < b);
	REQUIRE(a <= b);
	REQUIRE(a <= a2);
	REQUIRE(b > a);
	REQUIRE(a == a2);
	REQUIRE_FALSE(a == b);
	REQUIRE_FALSE(a > a2);

	SECTION("higher bit levels dominate, xMin ranks first within a level")
	{
		// the top bit of xMin is the key's most significant bit, so it beats everything else
		REQUIRE(ZOrderRect2DU16(0x7FFF, 0xFFFF, 0xFFFF, 0xFFFF) < ZOrderRect2DU16(0x8000, 0, 0, 0));

		// within one bit level the weights run yMax < yMin < xMax < xMin
		REQUIRE(ZOrderRect2DU16(0, 0, 0, 1) < ZOrderRect2DU16(0, 1, 0, 0)); // yMax < yMin
		REQUIRE(ZOrderRect2DU16(0, 1, 0, 0) < ZOrderRect2DU16(0, 0, 1, 0)); // yMin < xMax
		REQUIRE(ZOrderRect2DU16(0, 0, 1, 0) < ZOrderRect2DU16(1, 0, 0, 0)); // xMax < xMin
		REQUIRE(ZOrderRect2DU16(0, 1, 1, 1) < ZOrderRect2DU16(1, 0, 0, 0)); // xMin outranks the rest

		// but a lower level never outranks a higher one
		REQUIRE(ZOrderRect2DU16(1, 1, 1, 1) < ZOrderRect2DU16(0, 0, 0, 2)); // level 0 < level 1
	}

	SECTION("sorting with ZRect16Comp::LE matches operator<")
	{
		std::vector<ZOrderRect2DU16> v;
		for (int i = 0; i < 2000; ++i)
			v.emplace_back(RandomValue<uint16_t>(), RandomValue<uint16_t>(), RandomValue<uint16_t>(), RandomValue<uint16_t>());

		ZRect16Comp comp;
		std::sort(v.begin(), v.end(), comp);
		for (size_t i = 1; i < v.size(); ++i)
		{
			REQUIRE(v[i - 1].m_nZValue <= v[i].m_nZValue);
			REQUIRE_FALSE(comp.LE(v[i], v[i - 1]));
		}
		REQUIRE(comp.EQ(v[0], v[0]));
	}
}

TEST_CASE("ZOrderRect2DU32 ordering compares the high word first", "[spatial][zrect][u32]")
{
	ZOrderRect2DU32 lowBig, highSmall;
	lowBig.m_nZValue[1] = 0; lowBig.m_nZValue[0] = ~0ull;
	highSmall.m_nZValue[1] = 1; highSmall.m_nZValue[0] = 0;

	REQUIRE(lowBig < highSmall);
	REQUIRE(highSmall > lowBig);
	REQUIRE_FALSE(highSmall < lowBig);
	REQUIRE_FALSE(lowBig == highSmall);

	SECTION("ZRect32Comp sorts by the whole 128-bit value")
	{
		std::vector<ZOrderRect2DU32> v;
		for (int i = 0; i < 2000; ++i)
			v.emplace_back(RandomValue<uint32_t>(), RandomValue<uint32_t>(), RandomValue<uint32_t>(), RandomValue<uint32_t>());
		std::sort(v.begin(), v.end(), ZRect32Comp());

		ZRect32Comp comp;
		for (size_t i = 1; i < v.size(); ++i)
		{
			const bool ordered = v[i - 1].m_nZValue[1] < v[i].m_nZValue[1] ||
				(v[i - 1].m_nZValue[1] == v[i].m_nZValue[1] && v[i - 1].m_nZValue[0] <= v[i].m_nZValue[0]);
			REQUIRE(ordered);
			REQUIRE_FALSE(comp.LE(v[i], v[i - 1]));
		}
	}
}

TEST_CASE("ZOrderRect2DU64 ordering compares the most significant word first", "[spatial][zrect][u64]")
{
	ZOrderRect2DU64 a, b;
	a.m_nZValue[3] = 0; a.m_nZValue[0] = ~0ull;
	b.m_nZValue[3] = 1;

	REQUIRE(a < b);
	REQUIRE(a.less(b));
	REQUIRE_FALSE(b < a);
	REQUIRE(a <= b);
	REQUIRE_FALSE(b <= a);
	REQUIRE(a == a);

	SECTION("ZRect64Comp sorts consistently with operator<")
	{
		std::vector<ZOrderRect2DU64> v;
		for (int i = 0; i < 1000; ++i)
			v.emplace_back(RandomValue<uint64_t>(), RandomValue<uint64_t>(), RandomValue<uint64_t>(), RandomValue<uint64_t>());
		std::sort(v.begin(), v.end(), ZRect64Comp());

		ZRect64Comp comp;
		for (size_t i = 1; i < v.size(); ++i)
		{
			REQUIRE_FALSE(v[i] < v[i - 1]);
			REQUIRE_FALSE(comp.LE(v[i], v[i - 1]));
			REQUIRE(comp.EQ(v[i], v[i]));
		}
	}
}

TEST_CASE("ZOrderRect2DU64 operator> is strict", "[spatial][zrect][u64]")
{
	ZOrderRect2DU64 z(1, 2, 3, 4);
	REQUIRE_FALSE(z > z);

	// note: `small` is a macro in the Windows SDK (rpcndr.h defines it as char)
	ZOrderRect2DU64 lower(0, 0, 0, 0), upper(1, 1, 1, 1);
	REQUIRE(upper > lower);
	REQUIRE_FALSE(lower > upper);
	REQUIRE((upper > lower) == (lower < upper));

	for (int i = 0; i < 5000; ++i)
	{
		ZOrderRect2DU64 a, b;
		for (int w = 0; w < 4; ++w)
		{
			a.m_nZValue[w] = RandomValue<uint64_t>() & 3;
			b.m_nZValue[w] = RandomValue<uint64_t>() & 3;
		}
		REQUIRE((a > b) == (b < a));
		REQUIRE_FALSE(a > a);
	}
}

TEST_CASE("ZOrderRect2DU32 operator<= is consistent with < and ==", "[spatial][zrect][u32]")
{
	ZOrderRect2DU32 lowBig, highSmall;
	lowBig.m_nZValue[1] = 0; lowBig.m_nZValue[0] = 5;
	highSmall.m_nZValue[1] = 1; highSmall.m_nZValue[0] = 0;

	REQUIRE(lowBig <= highSmall);       // the high word decides
	REQUIRE_FALSE(highSmall <= lowBig);
	REQUIRE(lowBig <= lowBig);

	for (int i = 0; i < 5000; ++i)
	{
		// few distinct high words, so equal-high-word pairs come up often
		ZOrderRect2DU32 a, b;
		a.m_nZValue[1] = RandomValue<uint64_t>() & 3; a.m_nZValue[0] = RandomValue<uint64_t>() & 7;
		b.m_nZValue[1] = RandomValue<uint64_t>() & 3; b.m_nZValue[0] = RandomValue<uint64_t>() & 7;
		REQUIRE((a <= b) == (a < b || a == b));
		REQUIRE((a <= b) == !(b < a));
	}
}

// ---------------------------------------------------------------------------
// Arithmetic
// ---------------------------------------------------------------------------

TEST_CASE("Rect Z-order arithmetic carries between words", "[spatial][zrect]")
{
	SECTION("U16 wraps like uint64_t")
	{
		REQUIRE((ZOrderRect2DU16(static_cast<uint64_t>(100)) - ZOrderRect2DU16(static_cast<uint64_t>(30))).m_nZValue == 70ull);
		REQUIRE((ZOrderRect2DU16(static_cast<uint64_t>(100)) + ZOrderRect2DU16(static_cast<uint64_t>(30))).m_nZValue == 130ull);
		ZOrderRect2DU16 a(static_cast<uint64_t>(1));
		a += ZOrderRect2DU16(static_cast<uint64_t>(2));
		REQUIRE(a.m_nZValue == 3ull);
	}

	SECTION("U32 carries out of the low word")
	{
		ZOrderRect2DU32 a, one;
		a.m_nZValue[0] = ~0ull;
		one.m_nZValue[0] = 1;

		const ZOrderRect2DU32 sum = a + one;
		REQUIRE(sum.m_nZValue[1] == 1ull);
		REQUIRE(sum.m_nZValue[0] == 0ull);
		REQUIRE((sum - one) == a);
	}

	SECTION("U64 carries through all four words")
	{
		ZOrderRect2DU64 a, one;
		a.m_nZValue[0] = ~0ull;
		a.m_nZValue[1] = ~0ull;
		a.m_nZValue[2] = ~0ull;
		one.m_nZValue[0] = 1;

		const ZOrderRect2DU64 sum = a + one;
		REQUIRE(sum.m_nZValue[3] == 1ull);
		REQUIRE(sum.m_nZValue[2] == 0ull);
		REQUIRE(sum.m_nZValue[1] == 0ull);
		REQUIRE(sum.m_nZValue[0] == 0ull);

		ZOrderRect2DU64 acc = a;
		acc += one;
		REQUIRE(acc == sum);
		REQUIRE((sum - one) == a);
	}
}

// ---------------------------------------------------------------------------
// IsInRect: overlap between the stored rect and a query rect
// ---------------------------------------------------------------------------

TEST_CASE("Rect Z-order IsInRect reports overlap with the query rect", "[spatial][zrect][rect]")
{
	const TRect2Du16 query(100, 100, 200, 200);

	SECTION("overlapping, contained and containing rects all count")
	{
		REQUIRE(ZOrderRect2DU16(150, 150, 250, 250).IsInRect(query)); // partial overlap
		REQUIRE(ZOrderRect2DU16(120, 120, 180, 180).IsInRect(query)); // inside the query
		REQUIRE(ZOrderRect2DU16(0, 0, 1000, 1000).IsInRect(query));   // contains the query
		REQUIRE(ZOrderRect2DU16(100, 100, 200, 200).IsInRect(query)); // identical
		REQUIRE(ZOrderRect2DU16(150, 150, 150, 150).IsInRect(query)); // degenerate, inside
	}

	SECTION("disjoint rects do not")
	{
		REQUIRE_FALSE(ZOrderRect2DU16(300, 300, 400, 400).IsInRect(query));
		REQUIRE_FALSE(ZOrderRect2DU16(0, 0, 50, 50).IsInRect(query));
		REQUIRE_FALSE(ZOrderRect2DU16(100, 300, 200, 400).IsInRect(query)); // same x, above
	}

	SECTION("rects that only touch an edge or corner do not count as overlapping")
	{
		// isIntersection uses strict inequalities, and neither rect contains the other
		REQUIRE_FALSE(ZOrderRect2DU16(200, 100, 300, 200).IsInRect(query)); // shares the right edge
		REQUIRE_FALSE(ZOrderRect2DU16(200, 200, 300, 300).IsInRect(query)); // shares a corner
	}

	SECTION("U32 and U64 behave the same way")
	{
		const TRect2Du32 q32(100, 100, 200, 200);
		REQUIRE(ZOrderRect2DU32(150, 150, 250, 250).IsInRect(q32));
		REQUIRE(ZOrderRect2DU32(120, 120, 180, 180).IsInRect(q32));
		REQUIRE_FALSE(ZOrderRect2DU32(300, 300, 400, 400).IsInRect(q32));

		const TRect2Du64 q64(100, 100, 200, 200);
		REQUIRE(ZOrderRect2DU64(150, 150, 250, 250).IsInRect(q64));
		REQUIRE(ZOrderRect2DU64(120, 120, 180, 180).IsInRect(q64));
		REQUIRE_FALSE(ZOrderRect2DU64(300, 300, 400, 400).IsInRect(q64));
	}

	SECTION("random rects against the plain geometric predicates")
	{
		std::mt19937 rng(21);
		std::uniform_int_distribution<int> d(0, 300);
		for (int i = 0; i < 20000; ++i)
		{
			uint16_t xMin = uint16_t(d(rng)), xMax = uint16_t(d(rng));
			uint16_t yMin = uint16_t(d(rng)), yMax = uint16_t(d(rng));
			if (xMin > xMax) std::swap(xMin, xMax);
			if (yMin > yMax) std::swap(yMin, yMax);

			TRect2Du16 stored(xMin, yMin, xMax, yMax);
			TRect2Du16 q = query;
			const bool expected = q.isIntersection(stored) || q.isInRect(stored) || stored.isInRect(q);
			REQUIRE(ZOrderRect2DU16(stored).IsInRect(query) == expected);
		}
	}
}
