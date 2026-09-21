#pragma once

// Shared includes and helpers for the CommonLib/compress/EntropyCoders Catch2 tests.

#ifdef _WIN32
#define NOMINMAX
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "CommonLib/CommonLib.h"
#include "CommonLib/exception/exc_base.h"
#include "CommonLib/stream/MemoryStream.h"
#include "CommonLib/stream/FixedMemoryStream.h"
#include "CommonLib/compress/EntropyCoders/BaseACCoder.h"
#include "CommonLib/compress/EntropyCoders/ArithmeticCoder.h"
#include "CommonLib/compress/EntropyCoders/RangeCoder.h"

#include <vector>
#include <string>
#include <cstdint>
#include <random>
#include <algorithm>
#include <type_traits>

namespace test_utils
{
	// The four encoder/decoder pairs the library ships as typedefs.
	struct SAC32 { typedef CommonLib::TACEncoder32    Encoder; typedef CommonLib::TACDecoder32    Decoder; static const char* Name() { return "TAC 32"; } };
	struct SAC64 { typedef CommonLib::TACEncoder64    Encoder; typedef CommonLib::TACDecoder64    Decoder; static const char* Name() { return "TAC 64"; } };
	struct SRC32 { typedef CommonLib::TRangeEncoder32 Encoder; typedef CommonLib::TRangeDecoder32 Decoder; static const char* Name() { return "TRange 32"; } };
	struct SRC64 { typedef CommonLib::TRangeEncoder64 Encoder; typedef CommonLib::TRangeDecoder64 Decoder; static const char* Name() { return "TRange 64"; } };

	// TCodeValue is private on TRangeEncoder, so derive it from the public MaxRange constant.
	template<class TCoder>
	using CodeValueOf = typename std::remove_const<decltype(TCoder::MaxRange)>::type;

	// Cumulative frequency table: cum[s] .. cum[s+1] is the range of symbol s, cum.back() the total.
	template<class TCodeValue>
	std::vector<TCodeValue> MakeCumulative(const std::vector<TCodeValue>& freqs)
	{
		std::vector<TCodeValue> cum(freqs.size() + 1, 0);
		for (size_t i = 0; i < freqs.size(); ++i)
			cum[i + 1] = cum[i] + freqs[i];
		return cum;
	}

	// Symbol whose cumulative range contains the frequency returned by GetFreq().
	template<class TCodeValue>
	size_t SymbolFromFreq(const std::vector<TCodeValue>& cum, TCodeValue freq)
	{
		// first entry strictly greater than freq, minus one
		auto it = std::upper_bound(cum.begin(), cum.end(), freq);
		REQUIRE(it != cum.begin());
		REQUIRE(it != cum.end());
		return size_t(it - cum.begin()) - 1;
	}

	// Encodes symbols with a static model and returns the produced bytes.
	template<class TPair>
	std::vector<byte_t> EncodeStatic(const std::vector<size_t>& symbols, const std::vector<CodeValueOf<typename TPair::Encoder>>& cum)
	{
		typedef typename TPair::Encoder Encoder;
		CommonLib::CWriteMemoryStream ws;
		Encoder enc;
		enc.SetStream(&ws);

		for (size_t s : symbols)
		{
			REQUIRE(s + 1 < cum.size());
			REQUIRE(enc.EncodeSymbol(cum[s], cum[s + 1], cum.back()));
		}
		REQUIRE(enc.EncodeFinish());

		return std::vector<byte_t>(ws.Buffer(), ws.Buffer() + ws.Pos());
	}

	// Decodes `count` symbols with a static model.
	template<class TPair>
	std::vector<size_t> DecodeStatic(const std::vector<byte_t>& bytes, const std::vector<CodeValueOf<typename TPair::Decoder>>& cum, size_t count)
	{
		typedef typename TPair::Decoder Decoder;
		typedef CodeValueOf<Decoder> TCodeValue;

		CommonLib::CReadMemoryStream rs;
		rs.AttachBuffer(const_cast<byte_t*>(bytes.data()), bytes.size());
		Decoder dec;
		dec.SetStream(&rs);
		dec.StartDecode();

		std::vector<size_t> out;
		out.reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			const TCodeValue freq = dec.GetFreq(cum.back());
			const size_t s = SymbolFromFreq(cum, freq);
			dec.DecodeSymbol(cum[s], cum[s + 1], cum.back());
			out.push_back(s);
		}
		return out;
	}

	// Deterministic pseudo-random symbols from a weighted alphabet.
	template<class TCodeValue>
	std::vector<size_t> RandomSymbols(const std::vector<TCodeValue>& freqs, size_t count, uint32_t seed)
	{
		std::vector<double> weights(freqs.begin(), freqs.end());
		std::mt19937 rng(seed);
		std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
		std::vector<size_t> out(count);
		for (size_t& s : out)
			s = dist(rng);
		return out;
	}
}
