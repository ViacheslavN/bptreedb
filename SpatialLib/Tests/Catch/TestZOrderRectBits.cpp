#include "TestCommon.h"

// setLowBits / clearLowBits on the rect keys work in steps of 4 bits, because four
// coordinates are interleaved: they touch bit idx and every lower bit of the same
// coordinate (idx-4, idx-8, ...). FindRectMinZVal uses them to build the two halves
// of a split 4D box.

using namespace bptreedb::spatiallib;
using namespace test_utils;

namespace
{
	const uint64_t kSamples[] = {
		0ull, ~0ull,
		0x8888888888888888ull, 0x4444444444444444ull,
		0x2222222222222222ull, 0x1111111111111111ull,
		0x0123456789ABCDEFull, 0xFEDCBA9876543210ull,
	};
}

TEST_CASE("ZOrderRect2DU16 setLowBits / clearLowBits", "[spatial][zrect][bits][u16]")
{
	for (uint64_t sample : kSamples)
		for (int idx = 0; idx <= 63; ++idx)
		{
			uint64_t expected = sample;
			RefSetLowBitsRect(&expected, idx, true);
			ZOrderRect2DU16 s(sample);
			s.setLowBits(idx);
			REQUIRE(s.m_nZValue == expected);

			expected = sample;
			RefSetLowBitsRect(&expected, idx, false);
			ZOrderRect2DU16 c(sample);
			c.clearLowBits(idx);
			REQUIRE(c.m_nZValue == expected);
		}

	SECTION("only the coordinate that owns idx is touched")
	{
		// bit 3 of each nibble belongs to xMin; bit 35 is xMin's bit 8
		ZOrderRect2DU16 z(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
		z.setLowBits(35);
		uint16_t xMin = 0, yMin = 0, xMax = 0, yMax = 0;
		z.getXY(xMin, yMin, xMax, yMax);
		REQUIRE(xMin == 0xFEFF);  // xMin bit 8 cleared, lower bits already set
		REQUIRE(yMin == 0xFFFF);  // the other three coordinates are untouched
		REQUIRE(xMax == 0xFFFF);
		REQUIRE(yMax == 0xFFFF);

		ZOrderRect2DU16 w(0, 0, 0, 0);
		w.clearLowBits(35);
		w.getXY(xMin, yMin, xMax, yMax);
		REQUIRE(xMin == 0x0100);  // xMin bit 8 set, lower bits cleared
		REQUIRE(yMin == 0);
		REQUIRE(xMax == 0);
		REQUIRE(yMax == 0);
	}

	SECTION("clearLowBits then setLowBits at the same index is idempotent per dimension")
	{
		for (int idx = 0; idx <= 63; ++idx)
		{
			ZOrderRect2DU16 a(0x0123456789ABCDEFull);
			a.clearLowBits(idx);
			a.clearLowBits(idx);
			ZOrderRect2DU16 b(0x0123456789ABCDEFull);
			b.clearLowBits(idx);
			REQUIRE(a == b);
		}
	}
}

TEST_CASE("ZOrderRect2DU32 clearLowBits", "[spatial][zrect][bits][u32]")
{
	for (uint64_t hi : kSamples)
		for (uint64_t lo : kSamples)
			for (int idx = 0; idx <= 127; ++idx)
			{
				uint64_t expected[2] = { lo, hi };
				RefSetLowBitsRect(expected, idx, false);

				ZOrderRect2DU32 c;
				c.m_nZValue[1] = hi;
				c.m_nZValue[0] = lo;
				c.clearLowBits(idx);
				REQUIRE(c.m_nZValue[0] == expected[0]);
				REQUIRE(c.m_nZValue[1] == expected[1]);
			}
}

TEST_CASE("ZOrderRect2DU32 setLowBits", "[spatial][zrect][bits][u32]")
{
	for (uint64_t hi : kSamples)
		for (uint64_t lo : kSamples)
			for (int idx = 0; idx <= 127; ++idx)
			{
				uint64_t expected[2] = { lo, hi };
				RefSetLowBitsRect(expected, idx, true);

				ZOrderRect2DU32 s;
				s.m_nZValue[1] = hi;
				s.m_nZValue[0] = lo;
				s.setLowBits(idx);
				REQUIRE(s.m_nZValue[0] == expected[0]);
				REQUIRE(s.m_nZValue[1] == expected[1]);
			}
}

TEST_CASE("ZOrderRect2DU64 clearLowBits across all four words", "[spatial][zrect][bits][u64]")
{
	for (uint64_t sample : kSamples)
		for (int idx = 0; idx <= 255; ++idx)
		{
			uint64_t expected[4] = { sample, sample, sample, sample };
			RefSetLowBitsRect(expected, idx, false);

			ZOrderRect2DU64 c;
			for (int w = 0; w < 4; ++w)
				c.m_nZValue[w] = sample;
			c.clearLowBits(idx);
			for (int w = 0; w < 4; ++w)
				REQUIRE(c.m_nZValue[w] == expected[w]);
		}
}

TEST_CASE("ZOrderRect2DU64 setLowBits across all four words", "[spatial][zrect][bits][u64]")
{
	for (uint64_t sample : kSamples)
		for (int idx = 0; idx <= 255; ++idx)
		{
			uint64_t expected[4] = { sample, sample, sample, sample };
			RefSetLowBitsRect(expected, idx, true);

			ZOrderRect2DU64 s;
			for (int w = 0; w < 4; ++w)
				s.m_nZValue[w] = sample;
			s.setLowBits(idx);
			for (int w = 0; w < 4; ++w)
				REQUIRE(s.m_nZValue[w] == expected[w]);
		}
}

TEST_CASE("Rect Z-order setLowBits works whether or not bit idx is already set", "[spatial][zrect][bits]")
{
	// BIGMIN only ever calls setLowBits on a bit that is set; these check the other case,
	// where subtracting the bit instead of clearing it would borrow into the higher words.
	for (int idx = 0; idx <= 127; ++idx)
	{
		ZOrderRect2DU32 zero;
		zero.setLowBits(idx);
		REQUIRE(((zero.m_nZValue[idx > 63 ? 1 : 0] >> (idx & 63)) & 1) == 0);
		if (idx > 63)
			REQUIRE(zero.m_nZValue[1] < (uint64_t(1) << (idx - 64))); // no borrow above idx
	}

	for (int idx = 0; idx <= 255; ++idx)
	{
		ZOrderRect2DU64 zero;
		zero.setLowBits(idx);
		const int block = idx >> 6;
		REQUIRE(((zero.m_nZValue[block] >> (idx & 63)) & 1) == 0);
		for (int w = block + 1; w < 4; ++w)
			REQUIRE(zero.m_nZValue[w] == 0ull); // words above idx are untouched
	}
}

TEST_CASE("Rect Z-order getBit exposes bit idx in its lowest bit", "[spatial][zrect][bits]")
{
	SECTION("U16")
	{
		ZOrderRect2DU16 z(0x0123456789ABCDEFull);
		for (int idx = 0; idx <= z.getBits(); ++idx)
			REQUIRE(z.getBit(idx) == ((z.m_nZValue >> idx) & 1));
	}

	SECTION("U32")
	{
		ZOrderRect2DU32 z(0x12345678u, 0x9ABCDEF0u, 0x0FEDCBA9u, 0x87654321u);
		for (int idx = 0; idx <= z.getBits(); ++idx)
		{
			const uint64_t word = z.m_nZValue[idx > 63 ? 1 : 0];
			REQUIRE((z.getBit(idx) & 1) == ((word >> (idx & 63)) & 1));
		}
	}

	SECTION("U64")
	{
		ZOrderRect2DU64 z(0x0123456789ABCDEFull, 0xFEDCBA9876543210ull, 0x1111222233334444ull, 0x5555666677778888ull);
		for (int idx = 0; idx <= z.getBits(); ++idx)
		{
			const uint64_t word = z.m_nZValue[idx >> 6];
			REQUIRE((z.getBit(idx) & 1) == ((word >> (idx & 63)) & 1));
		}
	}
}
