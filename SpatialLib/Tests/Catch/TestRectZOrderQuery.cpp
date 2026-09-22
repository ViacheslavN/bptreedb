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
// Rects that intersect each other and cross the window border in every direction
// ---------------------------------------------------------------------------

namespace
{
	// How a stored rect sits relative to the window. All coordinates in these fixtures are
	// even and the window bounds odd, so no rect merely touches a border.
	enum enRelation
	{
		relDisjoint,
		relInside,      // stored rect inside the window
		relContains,    // stored rect swallows the window
		relCrossLeft,   // sticks out past one border
		relCrossRight,
		relCrossBottom,
		relCrossTop,
		relCorner,      // overlaps one corner only
		relBand,        // crosses the window completely in one axis
		relCount
	};

	enRelation Classify(const SRect& r, const TRect2Du16& q)
	{
		if (!Overlaps(r, q))
			return relDisjoint;
		if (r.xMin >= q.m_minX && r.yMin >= q.m_minY && r.xMax <= q.m_maxX && r.yMax <= q.m_maxY)
			return relInside;
		if (r.xMin <= q.m_minX && r.yMin <= q.m_minY && r.xMax >= q.m_maxX && r.yMax >= q.m_maxY)
			return relContains;

		const bool spansX = r.xMin < q.m_minX && r.xMax > q.m_maxX;
		const bool spansY = r.yMin < q.m_minY && r.yMax > q.m_maxY;
		if (spansX || spansY)
			return relBand;

		const bool outX = r.xMin < q.m_minX || r.xMax > q.m_maxX;
		const bool outY = r.yMin < q.m_minY || r.yMax > q.m_maxY;
		if (outX && outY)
			return relCorner;
		if (r.xMin < q.m_minX)
			return relCrossLeft;
		if (r.xMax > q.m_maxX)
			return relCrossRight;
		if (r.yMin < q.m_minY)
			return relCrossBottom;
		return relCrossTop;
	}

	// A deliberately overlapping population: nested rects, long bands, corner clippers and
	// clusters of rects piled on the same spot.
	std::vector<SRect> BuildIntersectingRects(uint16_t worldMax, const TRect2Du16& q)
	{
		std::vector<SRect> v;
		auto add = [&](int xMin, int yMin, int xMax, int yMax) {
			// keep everything even and inside the world
			auto clamp = [&](int c) { return uint16_t(std::max(0, std::min<int>(c, worldMax)) & ~1); };
			v.push_back(SRect{ clamp(xMin), clamp(yMin), clamp(xMax), clamp(yMax) });
		};

		// nested rects sharing a centre: each contains the previous one
		const int cx = (q.m_minX + q.m_maxX) / 2, cy = (q.m_minY + q.m_maxY) / 2;
		for (int r = 2; r < 40; r += 2)
			add(cx - r, cy - r, cx + r, cy + r);

		// rects growing out of the window centre until they swallow the window
		for (int r = 2; r < 60; r += 2)
			add(cx - r * 2, cy - r, cx + r * 2, cy + r);

		// rects that swallow the whole window, growing outwards
		for (int d = 2; d < 60; d += 4)
			add(q.m_minX - d, q.m_minY - d, q.m_maxX + d, q.m_maxY + d);

		// bands crossing the whole window horizontally and vertically
		for (int y = 0; y < worldMax; y += 6)
			add(0, y, worldMax, y + 4);
		for (int x = 0; x < worldMax; x += 6)
			add(x, 0, x + 4, worldMax);

		// clippers on each border and corner of the window
		for (int d = 2; d < 30; d += 2)
		{
			add(q.m_minX - d, q.m_minY + 4, q.m_minX + d, q.m_maxY - 4); // left border
			add(q.m_maxX - d, q.m_minY + 4, q.m_maxX + d, q.m_maxY - 4); // right border
			add(q.m_minX + 4, q.m_minY - d, q.m_maxX - 4, q.m_minY + d); // bottom border
			add(q.m_minX + 4, q.m_maxY - d, q.m_maxX - 4, q.m_maxY + d); // top border
			add(q.m_minX - d, q.m_minY - d, q.m_minX + d, q.m_minY + d); // corners
			add(q.m_maxX - d, q.m_maxY - d, q.m_maxX + d, q.m_maxY + d);
			add(q.m_minX - d, q.m_maxY - d, q.m_minX + d, q.m_maxY + d);
			add(q.m_maxX - d, q.m_minY - d, q.m_maxX + d, q.m_minY + d);
		}

		// piles of random rects of wildly different sizes, so many of them overlap
		std::mt19937 rng(99);
		std::uniform_int_distribution<int> pos(0, worldMax);
		std::uniform_int_distribution<int> smallSize(2, 20);
		std::uniform_int_distribution<int> bigSize(20, 400);
		for (int i = 0; i < 4000; ++i)
		{
			const int x = pos(rng), y = pos(rng);
			const int w = (i % 4 == 0) ? bigSize(rng) : smallSize(rng);
			const int h = (i % 3 == 0) ? bigSize(rng) : smallSize(rng);
			add(x, y, x + w, y + h);
		}

		// drop anything that degenerated during clamping
		v.erase(std::remove_if(v.begin(), v.end(),
			[](const SRect& r) { return r.xMin > r.xMax || r.yMin > r.yMax; }), v.end());
		return v;
	}

	// How many other rects each rect intersects, sampled.
	double AverageMutualIntersections(const std::vector<SRect>& v, size_t sample)
	{
		size_t hits = 0, tried = 0;
		for (size_t i = 0; i < v.size() && tried < sample; i += 7, ++tried)
		{
			TRect2Du16 a(v[i].xMin, v[i].yMin, v[i].xMax, v[i].yMax);
			for (size_t j = 0; j < v.size(); ++j)
			{
				if (i == j)
					continue;
				if (Overlaps(v[j], a))
					++hits;
			}
		}
		return tried ? double(hits) / double(tried) : 0.0;
	}
}

TEST_CASE("Window query over rects that intersect each other", "[spatial][zrect][query][intersect]")
{
	const uint16_t worldMax = 600;
	const TRect2Du16 query(151, 151, 449, 449); // odd bounds, rects are even: no edge touching

	const std::vector<SRect> rects = BuildIntersectingRects(worldMax, query);
	const std::vector<ZOrderRect2DU16> keys = SortedKeys(rects);
	REQUIRE(keys.size() > 4000);

	// the data really does overlap itself
	REQUIRE(AverageMutualIntersections(rects, 200) > 5.0);

	const ZOrderRect2DU16 zKeyMin(0, 0, query.m_minX, query.m_minY);
	const ZOrderRect2DU16 zKeyMax(query.m_maxX, query.m_maxY, worldMax, worldMax);

	std::vector<uint64_t> expected;
	for (const SRect& r : rects)
		if (Overlaps(r, query))
			expected.push_back(ZOrderRect2DU16(r.xMin, r.yMin, r.xMax, r.yMax).m_nZValue);
	std::sort(expected.begin(), expected.end());
	REQUIRE_FALSE(expected.empty());

	SECTION("the fixture covers every way a rect can meet the window")
	{
		size_t seen[relCount] = { 0 };
		for (const SRect& r : rects)
			++seen[Classify(r, query)];

		for (int k = 0; k < relCount; ++k)
			REQUIRE(seen[k] > 0); // disjoint, inside, containing, each border, corners, bands
	}

	SECTION("box membership still matches the geometric overlap test")
	{
		for (const SRect& r : rects)
		{
			const bool inBox = r.xMin <= query.m_maxX && r.yMin <= query.m_maxY &&
				r.xMax >= query.m_minX && r.yMax >= query.m_minY;
			REQUIRE(inBox == Overlaps(r, query));
		}
	}

	SECTION("both scans report exactly the overlapping rects")
	{
		const SScan full = FullScan(keys, zKeyMin, zKeyMax, query);
		const SScan skip = SkipScan(keys, zKeyMin, zKeyMax, query);

		REQUIRE(SortedZ(full.found) == expected);
		REQUIRE(SortedZ(skip.found) == expected);
		REQUIRE(skip.visited <= full.visited);
		REQUIRE(skip.seeks > 0);
	}

	SECTION("duplicate rects all come back")
	{
		// several stored rects share the same key: the scan must not collapse them
		std::vector<SRect> withDuplicates = rects;
		for (int i = 0; i < 50; ++i)
			withDuplicates.push_back(SRect{ 200, 200, 300, 300 });

		const std::vector<ZOrderRect2DU16> dupKeys = SortedKeys(withDuplicates);
		const SScan skip = SkipScan(dupKeys, zKeyMin, zKeyMax, query);

		const uint64_t dupZ = ZOrderRect2DU16(200, 200, 300, 300).m_nZValue;
		REQUIRE(std::count(skip.found.begin(), skip.found.end(), dupZ) >= 50);
	}
}

// The scans a renderer actually writes: the filter is the geometric overlap test, not box
// membership. For the intersects box the two coincide, except for rects whose edge lies
// exactly on the window border - those are in the box but fail the strict overlap test.
// The two scans must still agree, or a tile would render different features depending on
// whether the index skipped or not.
namespace
{
	SScan FullScanGeometric(const std::vector<ZOrderRect2DU16>& keys, const ZOrderRect2DU16& zKeyMin,
		const ZOrderRect2DU16& zKeyMax, const TRect2Du16& query)
	{
		SScan s;
		for (auto it = std::lower_bound(keys.begin(), keys.end(), zKeyMin, ZRect16Comp());
			it != keys.end() && !(zKeyMax < *it); ++it)
		{
			++s.visited;
			if (it->IsInRect(query))
				s.found.push_back(it->m_nZValue);
		}
		return s;
	}

	SScan SkipScanGeometric(const std::vector<ZOrderRect2DU16>& keys, const ZOrderRect2DU16& zKeyMin,
		const ZOrderRect2DU16& zKeyMax, const TRect2Du16& query)
	{
		SScan s;
		auto it = std::lower_bound(keys.begin(), keys.end(), zKeyMin, ZRect16Comp());
		while (it != keys.end() && !(zKeyMax < *it))
		{
			++s.visited;
			if (it->IsInRect(query))
			{
				s.found.push_back(it->m_nZValue);
				++it;
				continue;
			}

			if (!(*it < zKeyMax))
				break; // nothing can follow the last key of the box

			ZOrderRect2DU16 next;
			if (!FindRectMinZVal(*it, zKeyMin, zKeyMax, next))
				break;

			++s.seeks;
			const auto prev = it;
			it = std::lower_bound(keys.begin(), keys.end(), next, ZRect16Comp());
			REQUIRE(it > prev);
		}
		return s;
	}
}

TEST_CASE("Both scans agree when rects touch the window border", "[spatial][zrect][query][intersect]")
{
	const uint16_t worldMax = 600;
	const TRect2Du16 fixtureQuery(151, 151, 449, 449);
	const std::vector<SRect> rects = BuildIntersectingRects(worldMax, fixtureQuery);
	const std::vector<ZOrderRect2DU16> keys = SortedKeys(rects);

	// even window bounds this time, so they line up with the rect coordinates and plenty of
	// rects touch the border without overlapping it
	const TRect2Du16 query(150, 150, 450, 450);

	size_t touching = 0;
	for (const SRect& r : rects)
	{
		const bool inBox = r.xMin <= query.m_maxX && r.yMin <= query.m_maxY &&
			r.xMax >= query.m_minX && r.yMax >= query.m_minY;
		if (inBox && !Overlaps(r, query))
			++touching;
	}
	REQUIRE(touching > 0); // the fixture really does exercise the edge case

	const ZOrderRect2DU16 zKeyMin(0, 0, query.m_minX, query.m_minY);
	const ZOrderRect2DU16 zKeyMax(query.m_maxX, query.m_maxY, worldMax, worldMax);

	const SScan full = FullScanGeometric(keys, zKeyMin, zKeyMax, query);
	const SScan skip = SkipScanGeometric(keys, zKeyMin, zKeyMax, query);

	// the property a renderer depends on: the same features either way
	REQUIRE(SortedZ(full.found) == SortedZ(skip.found));

	std::vector<uint64_t> expected;
	for (const SRect& r : rects)
		if (Overlaps(r, query))
			expected.push_back(ZOrderRect2DU16(r.xMin, r.yMin, r.xMax, r.yMax).m_nZValue);
	std::sort(expected.begin(), expected.end());
	REQUIRE(SortedZ(full.found) == expected);
	REQUIRE(skip.seeks > 0);
}

TEST_CASE("Full scan and skipping scan agree for windows all over the world", "[spatial][zrect][query][intersect]")
{
	const uint16_t worldMax = 600;
	const TRect2Du16 fixtureQuery(151, 151, 449, 449);
	const std::vector<SRect> rects = BuildIntersectingRects(worldMax, fixtureQuery);
	const std::vector<ZOrderRect2DU16> keys = SortedKeys(rects);

	// tiles laid edge to edge, the way a renderer walks a viewport
	const uint16_t tile = 64;
	size_t compared = 0;
	for (uint16_t x = 0; x + tile <= worldMax; x = uint16_t(x + tile))
		for (uint16_t y = 0; y + tile <= worldMax; y = uint16_t(y + tile))
		{
			const TRect2Du16 query(x, y, uint16_t(x + tile), uint16_t(y + tile));
			const ZOrderRect2DU16 zKeyMin(0, 0, query.m_minX, query.m_minY);
			const ZOrderRect2DU16 zKeyMax(query.m_maxX, query.m_maxY, worldMax, worldMax);

			const SScan full = FullScanGeometric(keys, zKeyMin, zKeyMax, query);
			const SScan skip = SkipScanGeometric(keys, zKeyMin, zKeyMax, query);
			REQUIRE(SortedZ(full.found) == SortedZ(skip.found));
			++compared;
		}
	REQUIRE(compared > 50);
}

TEST_CASE("Window queries at many positions over intersecting rects", "[spatial][zrect][query][intersect]")
{
	const uint16_t worldMax = 600;
	const TRect2Du16 fixtureQuery(151, 151, 449, 449);
	const std::vector<SRect> rects = BuildIntersectingRects(worldMax, fixtureQuery);
	const std::vector<ZOrderRect2DU16> keys = SortedKeys(rects);

	// slide a window around the world: every position must give the same answer as brute force
	std::mt19937 rng(5);
	std::uniform_int_distribution<int> pos(0, worldMax - 60);
	std::uniform_int_distribution<int> size(20, 300);

	for (int i = 0; i < 40; ++i)
	{
		const int x = pos(rng) | 1, y = pos(rng) | 1; // odd bounds again
		const int w = size(rng), h = size(rng);
		const TRect2Du16 query(uint16_t(x), uint16_t(y),
			uint16_t(std::min<int>(x + w, worldMax - 1) | 1), uint16_t(std::min<int>(y + h, worldMax - 1) | 1));

		std::vector<uint64_t> expected;
		for (const SRect& r : rects)
			if (Overlaps(r, query))
				expected.push_back(ZOrderRect2DU16(r.xMin, r.yMin, r.xMax, r.yMax).m_nZValue);
		std::sort(expected.begin(), expected.end());

		const ZOrderRect2DU16 zKeyMin(0, 0, query.m_minX, query.m_minY);
		const ZOrderRect2DU16 zKeyMax(query.m_maxX, query.m_maxY, worldMax, worldMax);

		const SScan skip = SkipScan(keys, zKeyMin, zKeyMax, query);
		REQUIRE(SortedZ(skip.found) == expected);
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
