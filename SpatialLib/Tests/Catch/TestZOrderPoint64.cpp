#include "TestCommon.h"

using namespace bptreedb::spatiallib;
using namespace test_utils;

namespace
{
	ZOrderPoint2DU64 MakeZ(uint64_t hi, uint64_t lo)
	{
		ZOrderPoint2DU64 z;
		z.m_nZValue[1] = hi;
		z.m_nZValue[0] = lo;
		return z;
	}

	// 128-bit references working on (hi, lo) pairs; bit idx 0..127, 64+ in hi.
	void RefSetLowBits128(uint64_t& hi, uint64_t& lo, int idx)
	{
		auto setBit = [&](int b, bool on) {
			uint64_t& w = b > 63 ? hi : lo;
			const uint64_t m = uint64_t(1) << (b & 63);
			w = on ? (w | m) : (w & ~m);
		};
		setBit(idx, false);
		for (int b = idx - 2; b >= 0; b -= 2)
			setBit(b, true);
	}

	void RefClearLowBits128(uint64_t& hi, uint64_t& lo, int idx)
	{
		auto setBit = [&](int b, bool on) {
			uint64_t& w = b > 63 ? hi : lo;
			const uint64_t m = uint64_t(1) << (b & 63);
			w = on ? (w | m) : (w & ~m);
		};
		setBit(idx, true);
		for (int b = idx - 2; b >= 0; b -= 2)
			setBit(b, false);
	}

	// Plain 128-bit add / sub on (hi, lo) for checking the carry handling.
	void RefAdd(uint64_t ahi, uint64_t alo, uint64_t bhi, uint64_t blo, uint64_t& rhi, uint64_t& rlo)
	{
		rlo = alo + blo;
		rhi = ahi + bhi + (rlo < alo ? 1 : 0);
	}

	void RefSub(uint64_t ahi, uint64_t alo, uint64_t bhi, uint64_t blo, uint64_t& rhi, uint64_t& rlo)
	{
		rlo = alo - blo;
		rhi = ahi - bhi - (alo < blo ? 1 : 0);
	}
}

TEST_CASE("ZOrderPoint2DU64 constants and default state", "[spatial][zorder][u64]")
{
	ZOrderPoint2DU64 z;
	REQUIRE(z.m_nZValue[0] == 0ull);
	REQUIRE(z.m_nZValue[1] == 0ull);
	REQUIRE(z.getBits() == 127);
	REQUIRE(ZOrderPoint2DU64::SizeInByte == 16);
	REQUIRE(sizeof(ZOrderPoint2DU64) == 16);
}

TEST_CASE("ZOrderPoint2DU64 splits the coordinates into a 128-bit Morton code", "[spatial][zorder][u64]")
{
	SECTION("low 32 bits of x/y go to word 0, high 32 bits to word 1")
	{
		ZOrderPoint2DU64 z(uint64_t(0x00000001FFFFFFFFull), uint64_t(0));
		REQUIRE(z.m_nZValue[0] == 0xAAAAAAAAAAAAAAAAull);
		REQUIRE(z.m_nZValue[1] == 2ull);

		ZOrderPoint2DU64 w(uint64_t(0), uint64_t(0x0000000100000001ull));
		REQUIRE(w.m_nZValue[0] == 1ull);
		REQUIRE(w.m_nZValue[1] == 1ull);
	}

	SECTION("random points match the reference and round-trip")
	{
		for (int i = 0; i < 100000; ++i)
		{
			const uint64_t x = RandomValue<uint64_t>(), y = RandomValue<uint64_t>();
			ZOrderPoint2DU64 z(x, y);
			REQUIRE(z.m_nZValue[0] == RefInterleave<uint64_t, uint32_t>(uint32_t(x), uint32_t(y)));
			REQUIRE(z.m_nZValue[1] == RefInterleave<uint64_t, uint32_t>(uint32_t(x >> 32), uint32_t(y >> 32)));

			uint64_t rx = 0, ry = 0;
			z.getXY(rx, ry);
			REQUIRE(rx == x);
			REQUIRE(ry == y);
		}
	}

	SECTION("extreme coordinates")
	{
		const uint64_t edges[] = { 0ull, 1ull, 0xFFFFFFFFull, 0x100000000ull, 0x7FFFFFFFFFFFFFFFull,
			0x8000000000000000ull, ~0ull };
		for (uint64_t x : edges)
			for (uint64_t y : edges)
			{
				ZOrderPoint2DU64::TPoint p{ 0, 0 };
				ZOrderPoint2DU64(ZOrderPoint2DU64::TPoint{ x, y }).getXY(p);
				REQUIRE(p.m_x == x);
				REQUIRE(p.m_y == y);
			}
	}
}

TEST_CASE("ZOrderPoint2DU64 ordering compares the high word first", "[spatial][zorder][u64]")
{
	const ZOrderPoint2DU64 lowBig = MakeZ(0, ~0ull);
	const ZOrderPoint2DU64 highSmall = MakeZ(1, 0);

	REQUIRE(lowBig < highSmall);
	REQUIRE(highSmall > lowBig);
	REQUIRE_FALSE(highSmall < lowBig);
	REQUIRE_FALSE(lowBig > highSmall);
	REQUIRE(lowBig == MakeZ(0, ~0ull));
	REQUIRE_FALSE(lowBig == highSmall);

	SECTION("ZPointComp64 sorts by the full 128-bit value")
	{
		std::vector<ZOrderPoint2DU64> v;
		for (int i = 0; i < 2000; ++i)
			v.emplace_back(RandomValue<uint64_t>() >> (i % 40), RandomValue<uint64_t>() >> (i % 40));
		std::sort(v.begin(), v.end(), ZPointComp64());

		ZPointComp64 comp;
		for (size_t i = 1; i < v.size(); ++i)
		{
			const bool ordered = v[i - 1].m_nZValue[1] < v[i].m_nZValue[1] ||
				(v[i - 1].m_nZValue[1] == v[i].m_nZValue[1] && v[i - 1].m_nZValue[0] <= v[i].m_nZValue[0]);
			REQUIRE(ordered);
			REQUIRE_FALSE(comp.LE(v[i], v[i - 1]));
			REQUIRE(comp.EQ(v[i], v[i]));
		}
	}
}

TEST_CASE("ZOrderPoint2DU64 operator<= is consistent with < and ==", "[spatial][zorder][u64]")
{
	REQUIRE(MakeZ(0, 5) <= MakeZ(1, 0));        // high word decides
	REQUIRE_FALSE(MakeZ(1, 0) <= MakeZ(0, 5));
	REQUIRE(MakeZ(3, 3) <= MakeZ(3, 3));
	REQUIRE(MakeZ(3, 2) <= MakeZ(3, 3));        // equal high word: low word decides
	REQUIRE_FALSE(MakeZ(3, 4) <= MakeZ(3, 3));

	for (int i = 0; i < 10000; ++i)
	{
		// few distinct high words so equal-high-word pairs are common
		const ZOrderPoint2DU64 a = MakeZ(RandomValue<uint64_t>() & 3, RandomValue<uint64_t>() & 7);
		const ZOrderPoint2DU64 b = MakeZ(RandomValue<uint64_t>() & 3, RandomValue<uint64_t>() & 7);
		REQUIRE((a <= b) == (a < b || a == b));
		REQUIRE((a <= b) == !(b < a));
	}
}

TEST_CASE("ZOrderPoint2DU64 arithmetic carries between the words", "[spatial][zorder][u64]")
{
	SECTION("carry out of the low word")
	{
		const ZOrderPoint2DU64 r = MakeZ(0, ~0ull) + MakeZ(0, 1);
		REQUIRE(r.m_nZValue[1] == 1ull);
		REQUIRE(r.m_nZValue[0] == 0ull);
	}

	SECTION("borrow into the low word")
	{
		const ZOrderPoint2DU64 r = MakeZ(1, 0) - MakeZ(0, 1);
		REQUIRE(r.m_nZValue[1] == 0ull);
		REQUIRE(r.m_nZValue[0] == ~0ull);
	}

	SECTION("+= matches +")
	{
		ZOrderPoint2DU64 a = MakeZ(5, ~0ull - 2);
		a += MakeZ(1, 10);
		REQUIRE(a == MakeZ(5, ~0ull - 2) + MakeZ(1, 10));
		REQUIRE(a.m_nZValue[1] == 7ull);
		REQUIRE(a.m_nZValue[0] == 7ull);
	}

	SECTION("random values against a reference")
	{
		for (int i = 0; i < 50000; ++i)
		{
			const uint64_t ahi = RandomValue<uint64_t>(), alo = RandomValue<uint64_t>();
			const uint64_t bhi = RandomValue<uint64_t>(), blo = RandomValue<uint64_t>();
			uint64_t hi = 0, lo = 0;

			RefAdd(ahi, alo, bhi, blo, hi, lo);
			const ZOrderPoint2DU64 sum = MakeZ(ahi, alo) + MakeZ(bhi, blo);
			REQUIRE(sum.m_nZValue[1] == hi);
			REQUIRE(sum.m_nZValue[0] == lo);

			RefSub(ahi, alo, bhi, blo, hi, lo);
			const ZOrderPoint2DU64 diff = MakeZ(ahi, alo) - MakeZ(bhi, blo);
			REQUIRE(diff.m_nZValue[1] == hi);
			REQUIRE(diff.m_nZValue[0] == lo);

			REQUIRE((sum - MakeZ(bhi, blo)) == MakeZ(ahi, alo));
		}
	}
}

TEST_CASE("ZOrderPoint2DU64 getBit exposes bit idx in its lowest bit", "[spatial][zorder][u64]")
{
	const ZOrderPoint2DU64 z = MakeZ(0x0123456789ABCDEFull, 0xFEDCBA9876543210ull);
	for (int idx = 0; idx <= z.getBits(); ++idx)
	{
		const uint64_t word = idx > 63 ? z.m_nZValue[1] : z.m_nZValue[0];
		REQUIRE((z.getBit(idx) & 1u) == ((word >> (idx & 63)) & 1u));
	}
}

TEST_CASE("ZOrderPoint2DU64 setLowBits / clearLowBits across both words", "[spatial][zorder][u64]")
{
	const uint64_t samples[] = { 0ull, ~0ull, 0xAAAAAAAAAAAAAAAAull, 0x5555555555555555ull, 0x0123456789ABCDEFull };

	for (uint64_t hiSample : samples)
		for (uint64_t loSample : samples)
			for (int idx = 0; idx <= 127; ++idx)
			{
				uint64_t hi = hiSample, lo = loSample;
				RefSetLowBits128(hi, lo, idx);
				ZOrderPoint2DU64 s = MakeZ(hiSample, loSample);
				s.setLowBits(idx);
				REQUIRE(s.m_nZValue[1] == hi);
				REQUIRE(s.m_nZValue[0] == lo);

				hi = hiSample, lo = loSample;
				RefClearLowBits128(hi, lo, idx);
				ZOrderPoint2DU64 c = MakeZ(hiSample, loSample);
				c.clearLowBits(idx);
				REQUIRE(c.m_nZValue[1] == hi);
				REQUIRE(c.m_nZValue[0] == lo);
			}
}

TEST_CASE("ZOrderPoint2DU64 IsInRect agrees with the decoded point", "[spatial][zorder][u64][rect]")
{
	const TRect2Du64 rect(1ull << 20, 1ull << 30, 1ull << 50, 1ull << 60);
	REQUIRE(ZOrderPoint2DU64(1ull << 20, 1ull << 30).IsInRect(rect));
	REQUIRE(ZOrderPoint2DU64(1ull << 50, 1ull << 60).IsInRect(rect));
	REQUIRE_FALSE(ZOrderPoint2DU64((1ull << 20) - 1, 1ull << 40).IsInRect(rect));
	REQUIRE_FALSE(ZOrderPoint2DU64(1ull << 40, (1ull << 60) + 1).IsInRect(rect));

	for (int i = 0; i < 20000; ++i)
	{
		const uint64_t x = RandomValue<uint64_t>() >> 12, y = RandomValue<uint64_t>() >> 2;
		REQUIRE(ZOrderPoint2DU64(x, y).IsInRect(rect) == rect.isPoinInRect(x, y));
	}
}
