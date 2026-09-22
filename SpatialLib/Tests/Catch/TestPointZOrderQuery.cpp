#include "TestCommon.h"

// Window query over a Z-ordered point set, the way a spatial B+tree scan works:
// seek to the Z value of the rect's low corner, then walk until the Z value of its
// high corner. A plain scan sees every key in that Z range, including the ones whose
// point lies outside the rect; a scan that calls FindMinZVal jumps over those runs.
// Both must report exactly the same points.

using namespace bptreedb::spatiallib;

namespace
{
	uint64_t ZValueOf(const ZOrderPoint2DU16& z) { return z.m_nZValue; }
	uint64_t ZValueOf(const ZOrderPoint2DU32& z) { return z.m_nZValue; }

	template<class TZVal, class TComp>
	std::vector<TZVal> BuildSortedGrid(uint32_t maxCoord, uint32_t step, TComp comp)
	{
		std::vector<TZVal> points;
		points.reserve((maxCoord / step + 1) * (maxCoord / step + 1));
		for (uint32_t x = 0; x <= maxCoord; x += step)
			for (uint32_t y = 0; y <= maxCoord; y += step)
				points.push_back(TZVal(x, y));

		std::sort(points.begin(), points.end(), comp);
		return points;
	}

	struct SScanResult
	{
		std::vector<uint64_t> found; // z values of the points inside the rect
		size_t visited = 0;          // entries the scan actually looked at
		size_t seeks = 0;            // FindMinZVal jumps
	};

	// Plain scan: every key between zMin and zMax.
	template<class TZVal, class TRect, class TComp>
	SScanResult FullScan(const std::vector<TZVal>& points, const TRect& rect, TComp comp)
	{
		const TZVal zMin(rect.m_minX, rect.m_minY);
		const TZVal zMax(rect.m_maxX, rect.m_maxY);

		SScanResult r;
		for (auto it = std::lower_bound(points.begin(), points.end(), zMin, comp);
			it != points.end() && !(zMax < *it); ++it)
		{
			++r.visited;
			if (it->IsInRect(rect))
				r.found.push_back(ZValueOf(*it));
		}
		return r;
	}

	// Skipping scan: on a key outside the rect, ask FindMinZVal for the next Z value
	// inside it and seek there.
	template<class TZVal, class TRect, class TComp>
	SScanResult SkipScan(const std::vector<TZVal>& points, const TRect& rect, TComp comp)
	{
		const TZVal zMin(rect.m_minX, rect.m_minY);
		const TZVal zMax(rect.m_maxX, rect.m_maxY);

		SScanResult r;
		auto it = std::lower_bound(points.begin(), points.end(), zMin, comp);
		while (it != points.end() && !(zMax < *it))
		{
			++r.visited;
			if (it->IsInRect(rect))
			{
				r.found.push_back(ZValueOf(*it));
				++it;
				continue;
			}

			TZVal next;
			if (!FindMinZVal(*it, zMin, zMax, next))
				break; // nothing inside the rect can follow this key

			REQUIRE(*it < next); // the seek must move forward, or the scan would not terminate
			++r.seeks;
			const auto prev = it;
			it = std::lower_bound(points.begin(), points.end(), next, comp);
			REQUIRE(it > prev);
		}
		return r;
	}

	// Reference: every grid point inside the rect, whatever its Z value.
	template<class TZVal, class TRect>
	std::vector<uint64_t> BruteForce(const std::vector<TZVal>& points, const TRect& rect)
	{
		std::vector<uint64_t> found;
		for (const auto& p : points)
			if (p.IsInRect(rect))
				found.push_back(ZValueOf(p));
		std::sort(found.begin(), found.end());
		return found;
	}
}

TEST_CASE("Window query over a Z-ordered point grid (32 bit)", "[spatial][zorder][query][u32]")
{
	// 2000 x 2000 km-ish extent on a 4 km grid: 251001 points, as in the original test
	const uint32_t maxCoord = 2000000;
	const uint32_t step = 4000;
	ZPointComp<ZOrderPoint2DU32> comp; // operator() is non-const, so this cannot be const
	const std::vector<ZOrderPoint2DU32> points =
		BuildSortedGrid<ZOrderPoint2DU32>(maxCoord, step, comp);
	REQUIRE(points.size() == 501u * 501u);

	struct SCase { const char* name; TRect2Du32 rect; };
	const SCase cases[] = {
		{ "lower-left quadrant", TRect2Du32(0, 0, 1000000, 1000000) },
		{ "centre block",        TRect2Du32(800000, 800000, 1200000, 1200000) },
		{ "upper-right corner",  TRect2Du32(1500000, 1500000, 2000000, 2000000) },
		{ "wide flat strip",     TRect2Du32(100000, 900000, 1900000, 1000000) },
		{ "tall thin strip",     TRect2Du32(900000, 100000, 1000000, 1900000) },
		{ "whole extent",        TRect2Du32(0, 0, 2000000, 2000000) },
		{ "single grid cell",    TRect2Du32(400000, 400000, 400000, 400000) },
		{ "between grid lines",  TRect2Du32(400001, 400001, 403999, 403999) }, // holds no point
	};

	for (const SCase& c : cases)
	{
		INFO(c.name);
		const std::vector<uint64_t> expected = BruteForce(points, c.rect);

		const SScanResult full = FullScan(points, c.rect, comp);
		const SScanResult skip = SkipScan(points, c.rect, comp);

		// both scans must find exactly the points inside the rect
		std::vector<uint64_t> fullFound = full.found;
		std::sort(fullFound.begin(), fullFound.end());
		std::vector<uint64_t> skipFound = skip.found;
		std::sort(skipFound.begin(), skipFound.end());
		REQUIRE(fullFound == expected);
		REQUIRE(skipFound == expected);

		// skipping never looks at more keys than the plain scan
		REQUIRE(skip.visited <= full.visited);
	}
}

TEST_CASE("Skipping the Z-curve detours pays off for a quadrant query", "[spatial][zorder][query][u32]")
{
	ZPointComp<ZOrderPoint2DU32> comp; // operator() is non-const, so this cannot be const
	const std::vector<ZOrderPoint2DU32> points =
		BuildSortedGrid<ZOrderPoint2DU32>(2000000, 4000, comp);

	// The classic case: the query rect is a quadrant of the extent, and the Z curve keeps
	// leaving it. Here the plain scan reads about four keys for every key it keeps.
	const TRect2Du32 rect(300000, 300000, 900000, 900000);

	const SScanResult full = FullScan(points, rect, comp);
	const SScanResult skip = SkipScan(points, rect, comp);
	const size_t inRect = BruteForce(points, rect).size();

	REQUIRE(inRect > 0);
	REQUIRE(full.visited > inRect);           // the plain scan wastes work
	REQUIRE(skip.visited < full.visited / 2); // skipping cuts it by more than half
	REQUIRE(skip.seeks > 0);
	REQUIRE(skip.seeks <= skip.visited);

	// the skipping scan stays close to the number of keys it actually wants
	REQUIRE(skip.visited < inRect + inRect / 4);

	SECTION("a small rect away from the origin is where skipping helps most")
	{
		// the Z range of this rect covers a large part of the curve, but almost none of it
		// lies inside: the plain scan reads well over ten keys for every one it keeps
		const TRect2Du32 centre(800000, 800000, 1200000, 1200000);
		const SScanResult centreFull = FullScan(points, centre, comp);
		const SScanResult centreSkip = SkipScan(points, centre, comp);
		const size_t centreIn = BruteForce(points, centre).size();

		REQUIRE(centreFull.visited > 10 * centreIn);
		REQUIRE(centreSkip.visited < centreFull.visited / 8);
		REQUIRE(centreSkip.found.size() == centreIn);
	}
}

TEST_CASE("Window query over a Z-ordered point grid (16 bit)", "[spatial][zorder][query][u16]")
{
	const uint32_t maxCoord = 60000;
	const uint32_t step = 300;
	ZPointComp<ZOrderPoint2DU16> comp;
	const std::vector<ZOrderPoint2DU16> points =
		BuildSortedGrid<ZOrderPoint2DU16>(maxCoord, step, comp);

	const TRect2Du16 rects[] = {
		TRect2Du16(0, 0, 30000, 30000),
		TRect2Du16(12000, 45000, 21000, 54000),
		TRect2Du16(59700, 0, 60000, 60000),
		TRect2Du16(30000, 30000, 30000, 30000),
	};

	for (const TRect2Du16& rect : rects)
	{
		const std::vector<uint64_t> expected = BruteForce(points, rect);
		const SScanResult full = FullScan(points, rect, comp);
		const SScanResult skip = SkipScan(points, rect, comp);

		std::vector<uint64_t> fullFound = full.found;
		std::sort(fullFound.begin(), fullFound.end());
		std::vector<uint64_t> skipFound = skip.found;
		std::sort(skipFound.begin(), skipFound.end());
		REQUIRE(fullFound == expected);
		REQUIRE(skipFound == expected);
		REQUIRE(skip.visited <= full.visited);
	}
}
