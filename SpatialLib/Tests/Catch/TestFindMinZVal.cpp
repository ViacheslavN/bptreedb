#include "TestCommon.h"

using namespace bptreedb::spatiallib;

// FindMinZVal (BIGMIN): for a zVal that has left the query rect, find the next Z value
// that lies inside it again. Checked against a brute-force walk along the Z curve.

namespace
{
	// Brute force: first z > zVal (up to zMax) whose point is inside the rect.
	template<class TZVal, class TRect>
	bool RefNextInRect(const TRect& rect, const TZVal& zVal, const TZVal& zMax, TZVal& res)
	{
		TZVal z = zVal;
		const TZVal one(typename TZVal::ZValueType(1));
		while (z < zMax)
		{
			z += one;
			if (z.IsInRect(rect))
			{
				res = z;
				return true;
			}
		}
		return false;
	}

	struct SBox { uint32_t x0, y0, x1, y1; };

	std::vector<SBox> RandomBoxes(size_t count, uint32_t maxCoord, uint32_t seed)
	{
		std::mt19937 rng(seed);
		std::uniform_int_distribution<uint32_t> d(0, maxCoord);
		std::vector<SBox> boxes;
		for (size_t i = 0; i < count; ++i)
		{
			uint32_t x0 = d(rng), x1 = d(rng), y0 = d(rng), y1 = d(rng);
			if (x0 > x1) std::swap(x0, x1);
			if (y0 > y1) std::swap(y0, y1);
			boxes.push_back({ x0, y0, x1, y1 });
		}
		// a few fixed shapes: single cell, single row, single column, aligned block
		boxes.push_back({ 5, 5, 5, 5 });
		boxes.push_back({ 3, 9, 27, 9 });
		boxes.push_back({ 9, 3, 9, 27 });
		boxes.push_back({ 8, 8, 15, 15 });
		boxes.push_back({ 0, 0, maxCoord, maxCoord });
		return boxes;
	}

	// Walks every z in [zMin, zMax] that lies outside the rect and compares with the reference.
	template<class TZVal, class TRect, class TMake>
	void CheckAgainstBruteForce(const std::vector<SBox>& boxes, TMake make)
	{
		const TZVal one(typename TZVal::ZValueType(1));
		size_t checked = 0;
		for (const SBox& b : boxes)
		{
			const TZVal zMin = make(b.x0, b.y0);
			const TZVal zMax = make(b.x1, b.y1);
			const TRect rect(make.Coord(b.x0), make.Coord(b.y0), make.Coord(b.x1), make.Coord(b.y1));

			for (TZVal z = zMin; z < zMax; z += one)
			{
				if (z.IsInRect(rect))
					continue; // the function's contract: zVal is outside the rect

				TZVal expected;
				REQUIRE(RefNextInRect(rect, z, zMax, expected)); // zMax itself is always in the rect

				TZVal res;
				REQUIRE(FindMinZVal(z, zMin, zMax, res));
				REQUIRE(res == expected);
				REQUIRE(res.IsInRect(rect));
				REQUIRE(z < res);
				++checked;
			}
		}
		REQUIRE(checked > 0);
	}

	struct SMake16
	{
		ZOrderPoint2DU16 operator()(uint32_t x, uint32_t y) const { return ZOrderPoint2DU16(uint16_t(x), uint16_t(y)); }
		uint16_t Coord(uint32_t v) const { return uint16_t(v); }
	};

	struct SMake32
	{
		uint32_t offset;
		ZOrderPoint2DU32 operator()(uint32_t x, uint32_t y) const { return ZOrderPoint2DU32(offset + x, offset + y); }
		uint32_t Coord(uint32_t v) const { return offset + v; }
	};

	// ZOrderPoint2DU64 has no raw-value constructor / ZValueType, so it gets its own walk.
	struct SMake64
	{
		uint64_t offset;
		ZOrderPoint2DU64 operator()(uint32_t x, uint32_t y) const { return ZOrderPoint2DU64(offset + x, offset + y); }
		uint64_t Coord(uint32_t v) const { return offset + v; }
	};

	// For rects whose z-range is too wide to step through one value at a time (large
	// coordinates, or U64): take every point of a slightly padded area, sort by z, and use
	// the in-rect subset as the reference. Every zVal tested is one of those points.
	template<class TZVal, class TRect, class TMake, class TComp>
	void CheckByEnumeration(const std::vector<SBox>& boxes, TMake make, TComp comp)
	{
		size_t checked = 0;
		for (const SBox& b : boxes)
		{
			const TZVal zMin = make(b.x0, b.y0);
			const TZVal zMax = make(b.x1, b.y1);
			const TRect rect(make.Coord(b.x0), make.Coord(b.y0), make.Coord(b.x1), make.Coord(b.y1));

			std::vector<TZVal> zs;
			const uint32_t pad = 3;
			for (uint32_t x = (b.x0 > pad ? b.x0 - pad : 0); x <= b.x1 + pad; ++x)
				for (uint32_t y = (b.y0 > pad ? b.y0 - pad : 0); y <= b.y1 + pad; ++y)
					zs.push_back(make(x, y));
			std::sort(zs.begin(), zs.end(), comp);

			std::vector<TZVal> inRect;
			for (const auto& z : zs)
				if (z.IsInRect(rect))
					inRect.push_back(z);
			REQUIRE(!inRect.empty());

			for (const auto& z : zs)
			{
				if (z < zMin || !(z < zMax) || z.IsInRect(rect))
					continue;

				const auto it = std::upper_bound(inRect.begin(), inRect.end(), z, comp);
				REQUIRE(it != inRect.end()); // zMax is in the rect and follows z

				TZVal res;
				REQUIRE(FindMinZVal(z, zMin, zMax, res));
				REQUIRE(res == *it);
				++checked;
			}
		}
		REQUIRE(checked > 0);
	}
}

TEST_CASE("FindMinZVal matches brute force for ZOrderPoint2DU16", "[spatial][zorder][findmin][u16]")
{
	CheckAgainstBruteForce<ZOrderPoint2DU16, TRect2Du16>(RandomBoxes(300, 40, 11), SMake16());
}

TEST_CASE("FindMinZVal matches brute force for ZOrderPoint2DU32", "[spatial][zorder][findmin][u32]")
{
	SECTION("small coordinates")
	{
		CheckAgainstBruteForce<ZOrderPoint2DU32, TRect2Du32>(RandomBoxes(300, 40, 12), SMake32{ 0 });
	}

	SECTION("coordinates straddling 2^31: the split happens at the top bits")
	{
		// z-range of such a rect is ~2^63 wide, so it is checked by enumeration
		CheckByEnumeration<ZOrderPoint2DU32, TRect2Du32>(RandomBoxes(100, 40, 13), SMake32{ 0x7FFFFFF0u },
			ZPointComp<ZOrderPoint2DU32>());
	}
}

TEST_CASE("FindMinZVal matches brute force for ZOrderPoint2DU64", "[spatial][zorder][findmin][u64]")
{
	// U64 splits the coordinates into two 64-bit words; an offset just below 2^32 makes the
	// rect straddle that boundary so the search has to move between the words.
	SECTION("small coordinates")
	{
		CheckByEnumeration<ZOrderPoint2DU64, TRect2Du64>(RandomBoxes(100, 40, 14), SMake64{ 0ull }, ZPointComp64());
	}
	SECTION("straddling 2^32: split crosses the two words")
	{
		CheckByEnumeration<ZOrderPoint2DU64, TRect2Du64>(RandomBoxes(100, 40, 15), SMake64{ 0x00000000FFFFFFF0ull }, ZPointComp64());
	}
	SECTION("straddling 2^63: split at the very top bit")
	{
		CheckByEnumeration<ZOrderPoint2DU64, TRect2Du64>(RandomBoxes(100, 40, 16), SMake64{ 0x7FFFFFFFFFFFFFF0ull }, ZPointComp64());
	}
}

TEST_CASE("FindMinZVal jumps over a gap in the Z curve", "[spatial][zorder][findmin]")
{
	// rect x 2..5, y 2..5: the Z curve leaves it after (3,3) and re-enters later
	const TRect2Du16 rect(2, 2, 5, 5);
	const ZOrderPoint2DU16 zMin(uint16_t(2), uint16_t(2));
	const ZOrderPoint2DU16 zMax(uint16_t(5), uint16_t(5));

	const ZOrderPoint2DU16 last(uint16_t(3), uint16_t(3));   // last cell of the first quadrant block
	ZOrderPoint2DU16 out(uint32_t(last.m_nZValue + 1));      // first z after it: (0,4)-ish, outside
	REQUIRE_FALSE(out.IsInRect(rect));

	ZOrderPoint2DU16 res;
	REQUIRE(FindMinZVal(out, zMin, zMax, res));
	REQUIRE(res.IsInRect(rect));

	// nothing in the rect was skipped
	for (uint32_t z = out.m_nZValue + 1; z < res.m_nZValue; ++z)
		REQUIRE_FALSE(ZOrderPoint2DU16(z).IsInRect(rect));
}

TEST_CASE("FindMinZVal returns false when there is nothing to find", "[spatial][zorder][findmin]")
{
	const ZOrderPoint2DU32 zMin(10u, 10u);
	const ZOrderPoint2DU32 zMax(20u, 20u);
	ZOrderPoint2DU32 res(uint64_t(12345));

	SECTION("zVal == zMax: zMax is the last value in the rect, nothing follows it")
	{
		REQUIRE_FALSE(FindMinZVal(zMax, zMin, zMax, res));
		REQUIRE_NOTHROW(FindMinZVal(zMax, zMin, zMax, res));
	}

	SECTION("zVal beyond zMax")
	{
		REQUIRE_FALSE(FindMinZVal(ZOrderPoint2DU32(21u, 21u), zMin, zMax, res));
	}

	SECTION("zVal before zMin")
	{
		REQUIRE_FALSE(FindMinZVal(ZOrderPoint2DU32(9u, 9u), zMin, zMax, res));
	}

	SECTION("single-cell rect: zMin == zMax")
	{
		REQUIRE_FALSE(FindMinZVal(zMin, zMin, zMin, res));
	}

	REQUIRE(res.m_nZValue == 12345u); // untouched when returning false
}

TEST_CASE("FindMinZVal compiles and works for all three Z-order widths", "[spatial][zorder][findmin]")
{
	// a 2x2 rect at (1,1): z of (1,1) is 3, of (2,2) is 12; z 4..11 are partly outside
	{
		const ZOrderPoint2DU16 zMin(uint16_t(1), uint16_t(1)), zMax(uint16_t(2), uint16_t(2));
		ZOrderPoint2DU16 res;
		REQUIRE(FindMinZVal(ZOrderPoint2DU16(uint32_t(4)), zMin, zMax, res));
		REQUIRE(res == ZOrderPoint2DU16(uint16_t(1), uint16_t(2)));
	}
	{
		const ZOrderPoint2DU32 zMin(1u, 1u), zMax(2u, 2u);
		ZOrderPoint2DU32 res;
		REQUIRE(FindMinZVal(ZOrderPoint2DU32(uint64_t(4)), zMin, zMax, res));
		REQUIRE(res == ZOrderPoint2DU32(1u, 2u));
	}
	{
		const ZOrderPoint2DU64 zMin(uint64_t(1), uint64_t(1)), zMax(uint64_t(2), uint64_t(2));
		ZOrderPoint2DU64 z4;
		z4.m_nZValue[0] = 4;
		ZOrderPoint2DU64 res;
		REQUIRE(FindMinZVal(z4, zMin, zMax, res));
		REQUIRE(res == ZOrderPoint2DU64(uint64_t(1), uint64_t(2)));
	}
}
