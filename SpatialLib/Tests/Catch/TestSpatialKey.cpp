#include "TestCommon.h"

using namespace bptreedb::spatiallib;

// ---------------------------------------------------------------------------
// TPoint2D
// ---------------------------------------------------------------------------

TEST_CASE("TPoint2D equality", "[spatial][point2d]")
{
	TPoint2D32 a{ 1, 2 };
	TPoint2D32 b{ 1, 2 };
	TPoint2D32 c{ 2, 1 };
	TPoint2D32 d{ 1, 3 };

	REQUIRE(a == b);
	REQUIRE_FALSE(a != b);
	REQUIRE(a != c); // x and y are not interchangeable
	REQUIRE(a != d);
	REQUIRE_FALSE(a == d);

	TPoint2Du64 big{ std::numeric_limits<uint64_t>::max(), 0 };
	TPoint2Du64 big2{ std::numeric_limits<uint64_t>::max(), 0 };
	REQUIRE(big == big2);
}

TEST_CASE("TPoint2D typedefs use the expected coordinate types", "[spatial][point2d]")
{
	STATIC_REQUIRE(std::is_same<TPoint2D16::TPointType, int16_t>::value);
	STATIC_REQUIRE(std::is_same<TPoint2D32::TPointType, int32_t>::value);
	STATIC_REQUIRE(std::is_same<TPoint2D64::TPointType, int64_t>::value);
	STATIC_REQUIRE(std::is_same<TPoint2Du16::TPointType, uint16_t>::value);
	STATIC_REQUIRE(std::is_same<TPoint2Du32::TPointType, uint32_t>::value);
	STATIC_REQUIRE(std::is_same<TPoint2Du64::TPointType, uint64_t>::value);
	STATIC_REQUIRE(sizeof(TPoint2Du32) == 2 * sizeof(uint32_t));
}

// Points have no natural order (keys are ordered by their Z-order value): < and >
// are always false and <= / >= reduce to equality.
TEST_CASE("TPoint2D relational operators are consistent", "[spatial][point2d]")
{
	TPoint2D32 p{ 3, 4 };
	TPoint2D32 same{ 3, 4 };
	TPoint2D32 other{ 4, 3 };

	REQUIRE(p <= p);
	REQUIRE(p >= p);
	REQUIRE(p <= same);
	REQUIRE(p >= same);
	REQUIRE_FALSE(p < p);
	REQUIRE_FALSE(p > p);

	REQUIRE_FALSE(p < other);
	REQUIRE_FALSE(p > other);
	REQUIRE_FALSE(p <= other);
	REQUIRE_FALSE(p >= other);

	REQUIRE((p <= other) == (p < other || p == other));
	REQUIRE((p <= same) == (p < same || p == same));
}

// ---------------------------------------------------------------------------
// TRect2D
// ---------------------------------------------------------------------------

TEST_CASE("TRect2D construction, size and set", "[spatial][rect2d]")
{
	TRect2D32 r(1, 2, 11, 7);
	REQUIRE(r.m_minX == 1);
	REQUIRE(r.m_minY == 2);
	REQUIRE(r.m_maxX == 11);
	REQUIRE(r.m_maxY == 7);
	REQUIRE(r.width() == 10);
	REQUIRE(r.height() == 5);
	REQUIRE_FALSE(r.IsEmpty());

	r.set(-5, -5, 5, 5);
	REQUIRE(r == TRect2D32(-5, -5, 5, 5));
	REQUIRE(r.width() == 10);
	REQUIRE(r.height() == 10);

	SECTION("a single point is a valid, non-empty rect")
	{
		TRect2D32 p(3, 3, 3, 3);
		REQUIRE_FALSE(p.IsEmpty());
		REQUIRE(p.width() == 0);
		REQUIRE(p.height() == 0);
	}

	SECTION("max < min on either axis is empty")
	{
		REQUIRE(TRect2D32(5, 0, 4, 10).IsEmpty());
		REQUIRE(TRect2D32(0, 5, 10, 4).IsEmpty());
	}
}

TEST_CASE("TRect2D default constructed signed rect is empty", "[spatial][rect2d]")
{
	TRect2D32 r;
	REQUIRE(r.IsEmpty());
	TRect2D64 r64;
	REQUIRE(r64.IsEmpty());
	TRect2D16 r16;
	REQUIRE(r16.IsEmpty());
}

TEST_CASE("TRect2D default constructed unsigned rect is empty", "[spatial][rect2d]")
{
	TRect2Du16 r16;
	REQUIRE(r16.IsEmpty());
	TRect2Du32 r32;
	REQUIRE(r32.IsEmpty());
	TRect2Du64 r64;
	REQUIRE(r64.IsEmpty());

	r32.expand(5, 7);
	REQUIRE(r32 == TRect2Du32(5, 7, 5, 7));

	r64.expand(0, 0); // zero is a valid first point, not a sentinel
	REQUIRE(r64 == TRect2Du64(0, 0, 0, 0));
	REQUIRE_FALSE(r64.IsEmpty());
}

TEST_CASE("TRect2D empty rects have zero width and height", "[spatial][rect2d]")
{
	REQUIRE(TRect2D32().width() == 0);
	REQUIRE(TRect2D32().height() == 0);
	REQUIRE(TRect2Du32().width() == 0u);
	REQUIRE(TRect2Du64().height() == 0u);
	REQUIRE(TRect2D32(5, 5, 4, 10).width() == 0);  // max < min on x: empty
	REQUIRE(TRect2D32(5, 5, 4, 10).height() == 0);
}

TEST_CASE("TRect2D relational operators are consistent", "[spatial][rect2d]")
{
	const TRect2D32 a(0, 0, 10, 10);
	const TRect2D32 b(0, 0, 10, 10);
	const TRect2D32 c(1, 1, 5, 5);

	REQUIRE(a <= b);
	REQUIRE(a >= b);
	REQUIRE_FALSE(a < b);
	REQUIRE_FALSE(a > b);
	REQUIRE_FALSE(a <= c);
	REQUIRE_FALSE(a >= c);
}

TEST_CASE("TRect2D equality", "[spatial][rect2d]")
{
	TRect2D32 a(0, 0, 10, 10);
	REQUIRE(a == TRect2D32(0, 0, 10, 10));
	REQUIRE_FALSE(a != TRect2D32(0, 0, 10, 10));
	REQUIRE(a != TRect2D32(1, 0, 10, 10));
	REQUIRE(a != TRect2D32(0, 1, 10, 10));
	REQUIRE(a != TRect2D32(0, 0, 11, 10));
	REQUIRE(a != TRect2D32(0, 0, 10, 11));
}

TEST_CASE("TRect2D expand grows the rect to include points", "[spatial][rect2d]")
{
	TRect2D32 r; // empty
	r.expand(5, 5);
	REQUIRE(r == TRect2D32(5, 5, 5, 5));

	r.expand(10, 2);
	REQUIRE(r == TRect2D32(5, 2, 10, 5));

	r.expand(-3, 8);
	REQUIRE(r == TRect2D32(-3, 2, 10, 8));

	r.expand(0, 3); // inside: no change
	REQUIRE(r == TRect2D32(-3, 2, 10, 8));

	SECTION("expanding a single-point rect on both sides")
	{
		TRect2D32 p(0, 0, 0, 0);
		p.expand(-1, -1);
		p.expand(1, 1);
		REQUIRE(p == TRect2D32(-1, -1, 1, 1));
	}

	SECTION("the bounding box of random points contains every point")
	{
		TRect2D64 box;
		std::vector<TPoint2D64> pts;
		std::mt19937 rng(7);
		std::uniform_int_distribution<int64_t> dist(-1000000, 1000000);
		for (int i = 0; i < 500; ++i)
		{
			TPoint2D64 p{ dist(rng), dist(rng) };
			pts.push_back(p);
			box.expand(p.m_x, p.m_y);
		}
		for (const auto& p : pts)
			REQUIRE(box.isPoinInRect(p.m_x, p.m_y));

		// and it is tight: every edge is touched by some point
		auto touches = [&](auto pred) { return std::any_of(pts.begin(), pts.end(), pred); };
		REQUIRE(touches([&](const TPoint2D64& p) { return p.m_x == box.m_minX; }));
		REQUIRE(touches([&](const TPoint2D64& p) { return p.m_x == box.m_maxX; }));
		REQUIRE(touches([&](const TPoint2D64& p) { return p.m_y == box.m_minY; }));
		REQUIRE(touches([&](const TPoint2D64& p) { return p.m_y == box.m_maxY; }));
	}
}

TEST_CASE("TRect2D point containment: closed and open variants", "[spatial][rect2d]")
{
	const TRect2D32 r(0, 0, 10, 10);

	SECTION("isPoinInRect is inclusive of the border")
	{
		REQUIRE(r.isPoinInRect(5, 5));
		REQUIRE(r.isPoinInRect(0, 0));
		REQUIRE(r.isPoinInRect(10, 10));
		REQUIRE(r.isPoinInRect(0, 10));
		REQUIRE_FALSE(r.isPoinInRect(-1, 5));
		REQUIRE_FALSE(r.isPoinInRect(11, 5));
		REQUIRE_FALSE(r.isPoinInRect(5, -1));
		REQUIRE_FALSE(r.isPoinInRect(5, 11));
	}

	SECTION("isPoinInRectEQ is strict (border excluded)")
	{
		REQUIRE(r.isPoinInRectEQ(5, 5));
		REQUIRE(r.isPoinInRectEQ(1, 9));
		REQUIRE_FALSE(r.isPoinInRectEQ(0, 5));
		REQUIRE_FALSE(r.isPoinInRectEQ(10, 5));
		REQUIRE_FALSE(r.isPoinInRectEQ(5, 0));
		REQUIRE_FALSE(r.isPoinInRectEQ(5, 10));
	}
}

TEST_CASE("TRect2D rect containment: closed and open variants", "[spatial][rect2d]")
{
	const TRect2D32 outer(0, 0, 10, 10);

	SECTION("isInRect: this rect lies within the argument, borders allowed")
	{
		REQUIRE(TRect2D32(2, 2, 8, 8).isInRect(outer));
		REQUIRE(TRect2D32(0, 0, 10, 10).isInRect(outer));
		REQUIRE(TRect2D32(0, 3, 4, 10).isInRect(outer));
		REQUIRE_FALSE(TRect2D32(-1, 2, 8, 8).isInRect(outer));
		REQUIRE_FALSE(TRect2D32(2, 2, 11, 8).isInRect(outer));
		REQUIRE_FALSE(outer.isInRect(TRect2D32(2, 2, 8, 8)));
	}

	SECTION("isInRectEQ: this rect lies strictly inside the argument")
	{
		REQUIRE(TRect2D32(2, 2, 8, 8).isInRectEQ(outer));
		REQUIRE_FALSE(TRect2D32(0, 2, 8, 8).isInRectEQ(outer));
		REQUIRE_FALSE(TRect2D32(2, 2, 10, 8).isInRectEQ(outer));
		REQUIRE_FALSE(outer.isInRectEQ(outer));
	}
}

TEST_CASE("TRect2D intersection is strict: shared edges do not intersect", "[spatial][rect2d]")
{
	TRect2D32 a(0, 0, 10, 10);

	TRect2D32 overlap(5, 5, 15, 15);
	TRect2D32 inside(2, 2, 3, 3);
	TRect2D32 touchRight(10, 0, 20, 10);
	TRect2D32 touchCorner(10, 10, 20, 20);
	TRect2D32 apart(11, 0, 20, 10);
	TRect2D32 cross(-5, 4, 15, 6);
	TRect2D32 empty; // default: empty

	REQUIRE(a.isIntersection(overlap));
	REQUIRE(overlap.isIntersection(a));
	REQUIRE(a.isIntersection(inside));
	REQUIRE(inside.isIntersection(a));
	REQUIRE(a.isIntersection(cross));
	REQUIRE(a.isIntersection(a));

	REQUIRE_FALSE(a.isIntersection(touchRight));
	REQUIRE_FALSE(a.isIntersection(touchCorner));
	REQUIRE_FALSE(a.isIntersection(apart));
	REQUIRE_FALSE(a.isIntersection(empty));
	REQUIRE_FALSE(empty.isIntersection(a));
}
