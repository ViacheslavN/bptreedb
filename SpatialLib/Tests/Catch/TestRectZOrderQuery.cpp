#include "TestCommon.h"

// Window query over a set of rects stored under their 4D Z-order key, the way a
// spatial B+tree scan works: seek to the key of the query box's low corner, walk to
// the key of its high corner, and use FindRectMinZVal to jump over the stretches of
// the curve that fall outside the box.

using namespace bptreedb::spatiallib;

namespace
{
	struct SRect { uint16_t xMin, yMin, xMax, yMax; };

	// Tiles of `size`, laid out every `step` units, covering 0..worldMax.
	std::vector<SRect> BuildTiles(uint16_t worldMax, uint16_t step, uint16_t size)
	{
		std::vector<SRect> tiles;
		for (uint16_t x = 0; x + size <= worldMax; x = uint16_t(x + step))
			for (uint16_t y = 0; y + size <= worldMax; y = uint16_t(y + step))
				tiles.push_back(SRect{ x, y, uint16_t(x + size), uint16_t(y + size) });
		return tiles;
	}

	std::vector<ZOrderRect2DU16> SortedKeys(const std::vector<SRect>& rects)
	{
		std::vector<ZOrderRect2DU16> keys;
		keys.reserve(rects.size());
		for (const SRect& r : rects)
			keys.push_back(ZOrderRect2DU16(r.xMin, r.yMin, r.xMax, r.yMax));
		std::sort(keys.begin(), keys.end(), ZRect16Comp());
		return keys;
	}

	// The stored rect overlaps the query window (this is what ZOrderRect2DU16::IsInRect
	// reports: a strict intersection, or either rect containing the other).
	bool Overlaps(const SRect& r, const TRect2Du16& query)
	{
		TRect2Du16 stored(r.xMin, r.yMin, r.xMax, r.yMax);
		TRect2Du16 q = query;
		return q.isIntersection(stored) || q.isInRect(stored) || stored.isInRect(q);
	}

	struct SScan
	{
		std::vector<uint64_t> found; // z values of the rects the scan reported
		size_t visited = 0;
		size_t seeks = 0;
	};

	// Membership of the 4D box whose corners are zKeyMin and zKeyMax. This, not the
	// geometric overlap test, is what the key range selects and what BIGMIN jumps between:
	// a key can overlap the window and still sit outside the box (see the min-corner test).
	bool InKeyBox(const ZOrderRect2DU16& z, const ZOrderRect2DU16& zKeyMin, const ZOrderRect2DU16& zKeyMax)
	{
		uint16_t lo[4], hi[4], v[4];
		zKeyMin.getXY(lo[0], lo[1], lo[2], lo[3]);
		zKeyMax.getXY(hi[0], hi[1], hi[2], hi[3]);
		z.getXY(v[0], v[1], v[2], v[3]);
		for (int k = 0; k < 4; ++k)
			if (v[k] < lo[k] || v[k] > hi[k])
				return false;
		return true;
	}

	SScan FullScan(const std::vector<ZOrderRect2DU16>& keys, const ZOrderRect2DU16& zKeyMin,
		const ZOrderRect2DU16& zKeyMax, const TRect2Du16& query)
	{
		(void)query;
		SScan s;
		for (auto it = std::lower_bound(keys.begin(), keys.end(), zKeyMin, ZRect16Comp());
			it != keys.end() && !(zKeyMax < *it); ++it)
		{
			++s.visited;
			if (InKeyBox(*it, zKeyMin, zKeyMax))
				s.found.push_back(it->m_nZValue);
		}
		return s;
	}

	SScan SkipScan(const std::vector<ZOrderRect2DU16>& keys, const ZOrderRect2DU16& zKeyMin,
		const ZOrderRect2DU16& zKeyMax, const TRect2Du16& query)
	{
		(void)query;
		SScan s;
		auto it = std::lower_bound(keys.begin(), keys.end(), zKeyMin, ZRect16Comp());
		while (it != keys.end() && !(zKeyMax < *it))
		{
			++s.visited;
			if (InKeyBox(*it, zKeyMin, zKeyMax))
			{
				s.found.push_back(it->m_nZValue);
				++it;
				continue;
			}

			ZOrderRect2DU16 next;
			if (!FindRectMinZVal(*it, zKeyMin, zKeyMax, next))
				break;

			REQUIRE(*it < next); // must move forward, or the scan would spin
			++s.seeks;
			const auto prev = it;
			it = std::lower_bound(keys.begin(), keys.end(), next, ZRect16Comp());
			REQUIRE(it > prev);
		}
		return s;
	}

	std::vector<uint64_t> SortedZ(std::vector<uint64_t> v)
	{
		std::sort(v.begin(), v.end());
		return v;
	}
}

// ---------------------------------------------------------------------------
// The key range the original EmbDB test used: xMin and yMin inside the window
// ---------------------------------------------------------------------------

TEST_CASE("Rect window query over the min-corner key range", "[spatial][zrect][query]")
{
	// 6x6 rects every 2 units: they overlap each other, so plenty of them straddle the
	// window edge - that is what makes the min-corner range lossy
	const uint16_t worldMax = 60, step = 2, size = 6;
	const std::vector<SRect> tiles = BuildTiles(worldMax, step, size);
	const std::vector<ZOrderRect2DU16> keys = SortedKeys(tiles);
	REQUIRE(keys.size() > 500);

	// window bounds are odd while the rects sit on even coordinates, so no rect merely
	// touches the window edge and the geometric and 4D-box tests agree
	const TRect2Du16 query(13, 13, 41, 41);

	// zKeyMin = (qMinX, qMinY, 0, 0), zKeyMax = (qMaxX, qMaxY, worldMax, worldMax)
	const ZOrderRect2DU16 zKeyMin(query.m_minX, query.m_minY, 0, 0);
	const ZOrderRect2DU16 zKeyMax(query.m_maxX, query.m_maxY, worldMax, worldMax);

	const SScan full = FullScan(keys, zKeyMin, zKeyMax, query);
	const SScan skip = SkipScan(keys, zKeyMin, zKeyMax, query);

	SECTION("both scans report the same rects, and skipping reads no more keys")
	{
		REQUIRE(SortedZ(full.found) == SortedZ(skip.found));
		REQUIRE_FALSE(full.found.empty());
		REQUIRE(skip.visited <= full.visited);
		REQUIRE(skip.seeks > 0);
	}

	SECTION("every key the scan reports really does overlap the window")
	{
		for (uint64_t z : full.found)
		{
			uint16_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
			ZOrderRect2DU16(z).getXY(xMin, yMin, xMax, yMax);
			REQUIRE(Overlaps(SRect{ xMin, yMin, xMax, yMax }, query));
		}
	}

	SECTION("but it misses the rects that start left of or below the window")
	{
		// brute force: every rect that actually overlaps the window
		std::vector<uint64_t> overlapping;
		for (const SRect& r : tiles)
			if (Overlaps(r, query))
				overlapping.push_back(ZOrderRect2DU16(r.xMin, r.yMin, r.xMax, r.yMax).m_nZValue);

		const std::vector<uint64_t> scanned = SortedZ(full.found);
		const std::vector<uint64_t> expected = SortedZ(overlapping);

		// what the scan found is a subset of what overlaps ...
		REQUIRE(std::includes(expected.begin(), expected.end(), scanned.begin(), scanned.end()));
		// ... and it is a strict subset: rects starting left of or below the window are missed
		REQUIRE(scanned.size() < expected.size());

		std::vector<uint64_t> missed;
		std::set_difference(expected.begin(), expected.end(), scanned.begin(), scanned.end(),
			std::back_inserter(missed));
		REQUIRE_FALSE(missed.empty());
		for (uint64_t z : missed)
		{
			uint16_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
			ZOrderRect2DU16(z).getXY(xMin, yMin, xMax, yMax);
			REQUIRE((xMin < query.m_minX || yMin < query.m_minY));
		}
	}
}

// ---------------------------------------------------------------------------
// The key range that answers "which rects overlap the window?" in full
// ---------------------------------------------------------------------------

TEST_CASE("Rect window query over the full intersects box", "[spatial][zrect][query]")
{
	const uint16_t worldMax = 60, step = 2, size = 6;
	const std::vector<SRect> tiles = BuildTiles(worldMax, step, size);
	const std::vector<ZOrderRect2DU16> keys = SortedKeys(tiles);
	const TRect2Du16 query(13, 13, 41, 41);

	// intersects  <=>  xMin <= qMaxX && yMin <= qMaxY && xMax >= qMinX && yMax >= qMinY,
	// which is the 4D box from (0, 0, qMinX, qMinY) to (qMaxX, qMaxY, worldMax, worldMax)
	const ZOrderRect2DU16 zKeyMin(0, 0, query.m_minX, query.m_minY);
	const ZOrderRect2DU16 zKeyMax(query.m_maxX, query.m_maxY, worldMax, worldMax);

	std::vector<uint64_t> expected;
	for (const SRect& r : tiles)
		if (Overlaps(r, query))
			expected.push_back(ZOrderRect2DU16(r.xMin, r.yMin, r.xMax, r.yMax).m_nZValue);
	std::sort(expected.begin(), expected.end());
	REQUIRE_FALSE(expected.empty());

	const SScan full = FullScan(keys, zKeyMin, zKeyMax, query);
	const SScan skip = SkipScan(keys, zKeyMin, zKeyMax, query);

	SECTION("every overlapping rect is found, by both scans")
	{
		REQUIRE(SortedZ(full.found) == expected);
		REQUIRE(SortedZ(skip.found) == expected);
		REQUIRE(SortedZ(full.found) == SortedZ(skip.found));
	}

	SECTION("the 4D box membership matches the geometric test for this layout")
	{
		for (const SRect& r : tiles)
		{
			const bool inBox = r.xMin <= query.m_maxX && r.yMin <= query.m_maxY &&
				r.xMax >= query.m_minX && r.yMax >= query.m_minY;
			REQUIRE(inBox == Overlaps(r, query));
		}
	}

	SECTION("skipping reads fewer keys than the plain scan")
	{
		REQUIRE(skip.visited <= full.visited);
		REQUIRE(skip.seeks > 0);
		REQUIRE(full.visited > expected.size()); // the plain scan wastes work
	}
}

// ---------------------------------------------------------------------------
// 32-bit keys with the coordinates from the original EmbDB test
// ---------------------------------------------------------------------------

TEST_CASE("FindRectMinZVal on a 32-bit query box with large coordinates", "[spatial][zrect][query][u32]")
{
	// the query window from the original test
	const TRect2Du32 query(579557569u, 498287239u, 1738780183u, 1198956604u);
	const uint32_t worldMax = 0xFFFFFFFFu;

	const ZOrderRect2DU32 zKeyMin(0, 0, query.m_minX, query.m_minY);
	const ZOrderRect2DU32 zKeyMax(query.m_maxX, query.m_maxY, worldMax, worldMax);
	REQUIRE(zKeyMin < zKeyMax);

	auto inBox = [&](const ZOrderRect2DU32& z) {
		uint32_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
		z.getXY(xMin, yMin, xMax, yMax);
		return xMin <= query.m_maxX && yMin <= query.m_maxY &&
			xMax >= query.m_minX && yMax >= query.m_minY;
	};

	std::mt19937_64 rng(2024);
	size_t jumps = 0;
	for (int i = 0; i < 20000; ++i)
	{
		// a rect somewhere in the world, used as a key that the scan has landed on
		uint32_t xMin = uint32_t(rng()), xMax = uint32_t(rng());
		uint32_t yMin = uint32_t(rng()), yMax = uint32_t(rng());
		if (xMin > xMax) std::swap(xMin, xMax);
		if (yMin > yMax) std::swap(yMin, yMax);

		const ZOrderRect2DU32 z(xMin, yMin, xMax, yMax);
		if (z < zKeyMin || !(z < zKeyMax) || inBox(z))
			continue;

		ZOrderRect2DU32 res;
		REQUIRE(FindRectMinZVal(z, zKeyMin, zKeyMax, res));
		REQUIRE(z < res);        // always moves forward
		REQUIRE(inBox(res));     // and lands back inside the query box
		REQUIRE_FALSE(res < zKeyMin);
		REQUIRE_FALSE(zKeyMax < res);
		++jumps;
	}
	REQUIRE(jumps > 100);
}
