#include "TestCommon.h"

// FindRectMinZVal is the BIGMIN step for rect keys: given a zVal that has left the
// query region, it returns the next Z value inside it. The query region is a box in
// the 4D space (xMin, yMin, xMax, yMax) whose corners are zMin and zMax, so a value
// is inside when each of its four coordinates lies within that coordinate's range.

using namespace bptreedb::spatiallib;
using namespace test_utils;

namespace
{
	struct SBox4D
	{
		uint64_t lo[4];
		uint64_t hi[4];
	};

	// Builds every 4D point of the box (padded outwards), sorted by Z value, and checks
	// FindRectMinZVal against that list for every value that falls outside the box.
	template<class TZVal, class TMake, class TComp>
	size_t CheckBigMin(const SBox4D& box, TMake make, TComp comp, uint64_t pad)
	{
		std::vector<uint64_t> axis[4];
		for (int k = 0; k < 4; ++k)
		{
			const uint64_t from = box.lo[k] > pad ? box.lo[k] - pad : 0;
			for (uint64_t v = from; v <= box.hi[k] + pad; ++v)
				axis[k].push_back(v);
		}

		std::vector<TZVal> all;
		std::vector<TZVal> inBox;
		for (uint64_t a : axis[0])
			for (uint64_t b : axis[1])
				for (uint64_t c : axis[2])
					for (uint64_t d : axis[3])
					{
						const TZVal z = make(a, b, c, d);
						all.push_back(z);
						if (a >= box.lo[0] && a <= box.hi[0] && b >= box.lo[1] && b <= box.hi[1] &&
							c >= box.lo[2] && c <= box.hi[2] && d >= box.lo[3] && d <= box.hi[3])
							inBox.push_back(z);
					}
		std::sort(all.begin(), all.end(), comp);
		std::sort(inBox.begin(), inBox.end(), comp);
		REQUIRE(!inBox.empty());

		const TZVal zMin = make(box.lo[0], box.lo[1], box.lo[2], box.lo[3]);
		const TZVal zMax = make(box.hi[0], box.hi[1], box.hi[2], box.hi[3]);
		REQUIRE(zMin == inBox.front());
		REQUIRE(zMax == inBox.back());

		size_t checked = 0;
		for (const TZVal& z : all)
		{
			if (z < zMin || !(z < zMax))
				continue;
			if (std::binary_search(inBox.begin(), inBox.end(), z, comp))
				continue; // the contract: zVal lies outside the box

			const auto it = std::upper_bound(inBox.begin(), inBox.end(), z, comp);
			REQUIRE(it != inBox.end()); // zMax is inside the box and follows z

			TZVal res;
			REQUIRE(FindRectMinZVal(z, zMin, zMax, res));
			REQUIRE(res == *it);
			REQUIRE(z < res);
			++checked;
		}
		REQUIRE(checked > 0);
		return checked;
	}

	struct SMakeRect16
	{
		ZOrderRect2DU16 operator()(uint64_t a, uint64_t b, uint64_t c, uint64_t d) const
		{
			return ZOrderRect2DU16(uint16_t(a), uint16_t(b), uint16_t(c), uint16_t(d));
		}
	};

	struct SMakeRect32
	{
		uint32_t shift; // moves the coordinates up so higher key bits take part in the split
		ZOrderRect2DU32 operator()(uint64_t a, uint64_t b, uint64_t c, uint64_t d) const
		{
			return ZOrderRect2DU32(uint32_t(a) << shift, uint32_t(b) << shift, uint32_t(c) << shift, uint32_t(d) << shift);
		}
	};

	struct SMakeRect64
	{
		uint32_t shift;
		ZOrderRect2DU64 operator()(uint64_t a, uint64_t b, uint64_t c, uint64_t d) const
		{
			return ZOrderRect2DU64(a << shift, b << shift, c << shift, d << shift);
		}
	};

	struct SLess16 { bool operator()(const ZOrderRect2DU16& l, const ZOrderRect2DU16& r) const { return l < r; } };
	struct SLess32 { bool operator()(const ZOrderRect2DU32& l, const ZOrderRect2DU32& r) const { return l < r; } };
	struct SLess64 { bool operator()(const ZOrderRect2DU64& l, const ZOrderRect2DU64& r) const { return l < r; } };

	std::vector<SBox4D> SampleBoxes()
	{
		return {
			// xMin, yMin, xMax, yMax ranges — as produced by a window query on stored rects
			SBox4D{ { 0, 0, 2, 2 }, { 3, 3, 5, 5 } },
			SBox4D{ { 1, 1, 1, 1 }, { 2, 2, 4, 4 } },
			SBox4D{ { 0, 0, 0, 0 }, { 1, 1, 7, 7 } },
			SBox4D{ { 2, 3, 4, 5 }, { 3, 4, 5, 6 } },
			SBox4D{ { 0, 2, 3, 1 }, { 4, 4, 6, 6 } },
			SBox4D{ { 5, 5, 5, 5 }, { 6, 6, 6, 6 } },
		};
	}
}

TEST_CASE("FindRectMinZVal matches brute force for ZOrderRect2DU16", "[spatial][zrect][findmin][u16]")
{
	size_t total = 0;
	for (const SBox4D& box : SampleBoxes())
		total += CheckBigMin<ZOrderRect2DU16>(box, SMakeRect16(), SLess16(), 2);
	REQUIRE(total > 1000);
}

TEST_CASE("FindRectMinZVal matches brute force for ZOrderRect2DU32 in the low key bits", "[spatial][zrect][findmin][u32]")
{
	// coordinates stay small, so every split bit falls in the low word of the key
	size_t total = 0;
	for (const SBox4D& box : SampleBoxes())
		total += CheckBigMin<ZOrderRect2DU32>(box, SMakeRect32{ 0 }, SLess32(), 2);
	REQUIRE(total > 1000);
}

// Coordinates shifted up so the box splits on bits above 63, in the high word of the key.
// (setLowBits is buggy for a bit that is 0, but BIGMIN only ever calls it on a set bit.)
TEST_CASE("FindRectMinZVal for ZOrderRect2DU32 in the high key bits", "[spatial][zrect][findmin][u32]")
{
	for (const SBox4D& box : SampleBoxes())
		CheckBigMin<ZOrderRect2DU32>(box, SMakeRect32{ 17 }, SLess32(), 2);
}

TEST_CASE("FindRectMinZVal matches brute force for ZOrderRect2DU64", "[spatial][zrect][findmin][u64]")
{
	for (const SBox4D& box : SampleBoxes())
		CheckBigMin<ZOrderRect2DU64>(box, SMakeRect64{ 0 }, SLess64(), 2);
}

TEST_CASE("FindRectMinZVal returns false outside the key range", "[spatial][zrect][findmin]")
{
	const ZOrderRect2DU16 zMin(10, 10, 20, 20);
	const ZOrderRect2DU16 zMax(30, 30, 40, 40);
	ZOrderRect2DU16 res(uint64_t(12345));

	SECTION("below zMin")
	{
		REQUIRE_FALSE(FindRectMinZVal(ZOrderRect2DU16(0, 0, 0, 0), zMin, zMax, res));
		REQUIRE(res.m_nZValue == 12345ull); // untouched
	}

	SECTION("above zMax")
	{
		REQUIRE_FALSE(FindRectMinZVal(ZOrderRect2DU16(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF), zMin, zMax, res));
	}

	SECTION("zMin itself is inside the box, so it is not an error")
	{
		REQUIRE_NOTHROW(FindRectMinZVal(zMin, zMin, zMax, res));
	}
}

// zMax is the last key of the box: a scan that walks to it must be told "no next key"
// rather than have an exception thrown at it.
TEST_CASE("FindRectMinZVal returns false at zMax instead of throwing", "[spatial][zrect][findmin]")
{
	SECTION("ZOrderRect2DU16")
	{
		const ZOrderRect2DU16 zMin(10, 10, 20, 20);
		const ZOrderRect2DU16 zMax(30, 30, 40, 40);
		ZOrderRect2DU16 res(uint64_t(7));

		REQUIRE_NOTHROW(FindRectMinZVal(zMax, zMin, zMax, res));
		REQUIRE_FALSE(FindRectMinZVal(zMax, zMin, zMax, res));
		REQUIRE(res.m_nZValue == 7ull); // output untouched

		// a single-key box: zMin == zMax, so there is never a next key
		REQUIRE_FALSE(FindRectMinZVal(zMin, zMin, zMin, res));
		REQUIRE_NOTHROW(FindRectMinZVal(zMin, zMin, zMin, res));

		// many random boxes: walking onto zMax must never throw
		for (int i = 0; i < 2000; ++i)
		{
			uint16_t lo[4], hi[4];
			for (int k = 0; k < 4; ++k)
			{
				lo[k] = uint16_t(RandomValue<uint16_t>() & 0x3F);
				hi[k] = uint16_t(RandomValue<uint16_t>() & 0x3F);
				if (lo[k] > hi[k])
					std::swap(lo[k], hi[k]);
			}
			const ZOrderRect2DU16 a(lo[0], lo[1], lo[2], lo[3]);
			const ZOrderRect2DU16 b(hi[0], hi[1], hi[2], hi[3]);
			ZOrderRect2DU16 out;
			REQUIRE_NOTHROW(FindRectMinZVal(b, a, b, out));
			REQUIRE_FALSE(FindRectMinZVal(b, a, b, out));
		}
	}

	SECTION("ZOrderRect2DU32")
	{
		const ZOrderRect2DU32 zMin(10, 10, 20, 20);
		const ZOrderRect2DU32 zMax(30, 30, 40, 40);
		ZOrderRect2DU32 res;
		REQUIRE_NOTHROW(FindRectMinZVal(zMax, zMin, zMax, res));
		REQUIRE_FALSE(FindRectMinZVal(zMax, zMin, zMax, res));
		REQUIRE_FALSE(FindRectMinZVal(zMin, zMin, zMin, res));
	}

	SECTION("ZOrderRect2DU64")
	{
		const ZOrderRect2DU64 zMin(10, 10, 20, 20);
		const ZOrderRect2DU64 zMax(30, 30, 40, 40);
		ZOrderRect2DU64 res;
		REQUIRE_NOTHROW(FindRectMinZVal(zMax, zMin, zMax, res));
		REQUIRE_FALSE(FindRectMinZVal(zMax, zMin, zMax, res));
		REQUIRE_FALSE(FindRectMinZVal(zMin, zMin, zMin, res));
	}
}

// A scan that walks to the end of the box needs no guard of its own any more.
TEST_CASE("A scan can walk onto zMax without guarding the call", "[spatial][zrect][findmin]")
{
	const uint16_t worldMax = 64;
	const TRect2Du16 query(9, 9, 33, 33);
	const ZOrderRect2DU16 zKeyMin(0, 0, query.m_minX, query.m_minY);
	const ZOrderRect2DU16 zKeyMax(query.m_maxX, query.m_maxY, worldMax, worldMax);

	std::vector<ZOrderRect2DU16> keys;
	for (uint16_t x = 0; x + 4 <= worldMax; x = uint16_t(x + 4))
		for (uint16_t y = 0; y + 4 <= worldMax; y = uint16_t(y + 4))
			keys.push_back(ZOrderRect2DU16(x, y, uint16_t(x + 4), uint16_t(y + 4)));
	keys.push_back(zKeyMax); // the very last key of the box, and not a rect that overlaps
	std::sort(keys.begin(), keys.end(), ZRect16Comp());

	size_t found = 0, seeks = 0;
	auto it = std::lower_bound(keys.begin(), keys.end(), zKeyMin, ZRect16Comp());
	while (it != keys.end() && !(zKeyMax < *it))
	{
		if (it->IsInRect(query))
		{
			++found;
			++it;
			continue;
		}

		ZOrderRect2DU16 next;
		if (!FindRectMinZVal(*it, zKeyMin, zKeyMax, next)) // no guard needed here
			break;

		++seeks;
		it = std::lower_bound(keys.begin(), keys.end(), next, ZRect16Comp());
	}
	REQUIRE(found > 0);
	REQUIRE(seeks > 0);
}

TEST_CASE("FindRectMinZVal result re-enters the query box", "[spatial][zrect][findmin]")
{
	// A window query over stored rects: "which rects overlap (10,10)-(20,20)?" becomes
	// xMin <= 20, yMin <= 20, xMax >= 10, yMax >= 10 — a box in the 4D key space.
	const uint16_t worldMax = 40;
	const ZOrderRect2DU16 zMin(0, 0, 10, 10);
	const ZOrderRect2DU16 zMax(20, 20, worldMax, worldMax);

	auto inBox = [&](const ZOrderRect2DU16& z) {
		uint16_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
		z.getXY(xMin, yMin, xMax, yMax);
		return xMin <= 20 && yMin <= 20 && xMax >= 10 && yMax >= 10 && xMax <= worldMax && yMax <= worldMax;
	};

	size_t jumps = 0;
	for (uint16_t xMin = 0; xMin <= 20; xMin += 3)
		for (uint16_t yMin = 0; yMin <= 20; yMin += 3)
			for (uint16_t xMax = 10; xMax <= worldMax; xMax += 5)
				for (uint16_t yMax = 10; yMax <= worldMax; yMax += 5)
				{
					// walk a little past each stored rect to land on out-of-box keys
					const ZOrderRect2DU16 stored(xMin, yMin, xMax, yMax);
					const ZOrderRect2DU16 probe(stored.m_nZValue + 1);
					if (probe < zMin || !(probe < zMax) || inBox(probe))
						continue;

					ZOrderRect2DU16 res;
					REQUIRE(FindRectMinZVal(probe, zMin, zMax, res));
					REQUIRE(probe < res);
					REQUIRE(inBox(res));
					++jumps;
				}
	REQUIRE(jumps > 0);
}
