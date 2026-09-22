#pragma once

// Shared includes and reference helpers for the SpatialLib Catch2 tests.

#ifdef _WIN32
#define NOMINMAX
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "SpatialLib/SpatialKey.h"
#include "SpatialLib/ZCurve/Point/SpatialPointQuery.h"
#include "SpatialLib/ZCurve/Rect/SpatialRectQuery.h"

#include <vector>
#include <cstdint>
#include <limits>
#include <random>
#include <algorithm>

namespace test_utils
{
	// Bit-by-bit reference for the Morton / Z-order interleave used by SpatialLib:
	// bit i of y goes to bit 2*i, bit i of x goes to bit 2*i + 1.
	template<class TZ, class TCoord>
	TZ RefInterleave(TCoord x, TCoord y)
	{
		TZ z = 0;
		for (unsigned i = 0; i < sizeof(TCoord) * 8; ++i)
		{
			z |= TZ((uint64_t(y) >> i) & 1u) << (2 * i);
			z |= TZ((uint64_t(x) >> i) & 1u) << (2 * i + 1);
		}
		return z;
	}

	// Reference for setLowBits(idx) on a single word: bit idx becomes 0 and every
	// lower bit of the same dimension (idx-2, idx-4, ...) becomes 1. Other bits untouched.
	template<class TZ>
	TZ RefSetLowBits(TZ z, int idx)
	{
		z &= ~(TZ(1) << idx);
		for (int b = idx - 2; b >= 0; b -= 2)
			z |= TZ(1) << b;
		return z;
	}

	// Reference for clearLowBits(idx) on a single word: bit idx becomes 1 and every
	// lower bit of the same dimension becomes 0. Other bits untouched.
	template<class TZ>
	TZ RefClearLowBits(TZ z, int idx)
	{
		z |= TZ(1) << idx;
		for (int b = idx - 2; b >= 0; b -= 2)
			z &= ~(TZ(1) << b);
		return z;
	}

	// Rect keys interleave four coordinates: bit i of xMin lands on bit 4i+3, xMax on
	// 4i+2, yMin on 4i+1 and yMax on 4i+0 (so xMin is the most significant dimension).
	template<class TCoord>
	void RefInterleaveRect(TCoord xMin, TCoord yMin, TCoord xMax, TCoord yMax,
		uint64_t* words, unsigned wordCount)
	{
		for (unsigned w = 0; w < wordCount; ++w)
			words[w] = 0;

		const unsigned bits = sizeof(TCoord) * 8;
		for (unsigned i = 0; i < bits; ++i)
		{
			const uint64_t dim[4] = {
				(uint64_t(yMax) >> i) & 1, (uint64_t(yMin) >> i) & 1,
				(uint64_t(xMax) >> i) & 1, (uint64_t(xMin) >> i) & 1 };
			for (unsigned d = 0; d < 4; ++d)
			{
				if (!dim[d])
					continue;
				const unsigned bit = 4 * i + d;
				words[bit / 64] |= uint64_t(1) << (bit % 64);
			}
		}
	}

	// setLowBits(idx): bit idx becomes 0 and every lower bit of the same dimension
	// (idx-4, idx-8, ...) becomes 1. clearLowBits(idx) is the mirror image.
	inline void RefSetLowBitsRect(uint64_t* words, int idx, bool set)
	{
		auto put = [&](int b, bool on) {
			const uint64_t m = uint64_t(1) << (b % 64);
			if (on) words[b / 64] |= m; else words[b / 64] &= ~m;
		};
		put(idx, !set);
		for (int b = idx - 4; b >= 0; b -= 4)
			put(b, set);
	}

	inline std::mt19937_64& Rng()
	{
		static std::mt19937_64 rng(0x5EED5EEDull);
		return rng;
	}

	template<class T>
	T RandomValue()
	{
		return T(Rng()());
	}
}
