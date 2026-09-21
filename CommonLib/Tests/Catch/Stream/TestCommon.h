#pragma once

// Shared includes and helpers for the CommonLib/stream Catch2 tests.

#ifdef _WIN32
#define NOMINMAX
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "CommonLib/CommonLib.h"
#include "CommonLib/exception/exc_base.h"
#include "CommonLib/alloc/alloc.h"
#include "CommonLib/alloc/simpleAlloc.h"
#include "CommonLib/stream/stream.h"
#include "CommonLib/stream/MemoryStream.h"
#include "CommonLib/stream/FixedMemoryStream.h"

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <limits>
#include <algorithm>

namespace test_utils
{
	// Deterministic, non-trivial byte pattern of length n.
	inline std::vector<byte_t> MakePattern(size_t n, byte_t seed = 0)
	{
		std::vector<byte_t> v(n);
		for (size_t i = 0; i < n; ++i)
			v[i] = byte_t(seed + i * 7 + (i >> 3));
		return v;
	}

	inline bool SameBytes(const byte_t* p, const std::vector<byte_t>& v)
	{
		return p != nullptr && (v.empty() || std::memcmp(p, v.data(), v.size()) == 0);
	}

	// Attach a reader to exactly the bytes written so far by a memory writer.
	template<class TWriter>
	inline void AttachWritten(CommonLib::CReadMemoryStream& reader, TWriter& writer)
	{
		reader.AttachBuffer(writer.Buffer(), writer.Pos(), false);
	}

	// Runtime endianness check independent of IStream::IsBigEndian().
	inline bool HostIsBigEndian()
	{
		const uint32_t v = 0x01020304u;
		byte_t b[4];
		std::memcpy(b, &v, 4);
		return b[0] == 0x01;
	}

	// Simple POD used for the WriteT/ReadT template round-trips.
	struct SPod
	{
		int32_t a;
		uint16_t b;
		double c;
		byte_t d[5];

		bool operator==(const SPod& o) const
		{
			return a == o.a && b == o.b && c == o.c && std::memcmp(d, o.d, sizeof(d)) == 0;
		}
	};
}
