#include "TestCommon.h"

using namespace CommonLib;
using namespace test_utils;

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

TEST_CASE("Entropy coder constants", "[entropy][limits]")
{
	SECTION("MaxRange of the arithmetic coders is a quarter of the value range minus one")
	{
		REQUIRE(TACEncoder32::MaxRange == (uint32_t(1) << 14) - 1);
		REQUIRE(TACDecoder32::MaxRange == TACEncoder32::MaxRange);
		REQUIRE(TACEncoder64::MaxRange == (uint64_t(1) << 30) - 1);
		REQUIRE(TACDecoder64::MaxRange == TACEncoder64::MaxRange);
	}

	SECTION("MaxRange of the range coders is the Bottom normalisation bound")
	{
		REQUIRE(TRangeEncoder32::MaxRange == (uint32_t(1) << 16));
		REQUIRE(TRangeDecoder32::MaxRange == TRangeEncoder32::MaxRange);
		REQUIRE(TRangeEncoder64::MaxRange == (uint64_t(1) << 48));
		REQUIRE(TRangeDecoder64::MaxRange == TRangeEncoder64::MaxRange);
	}

	SECTION("GetAdditionalSize reports the flush overhead")
	{
		REQUIRE(TACEncoder32().GetAdditionalSize() == sizeof(uint32_t) + sizeof(byte_t));
		REQUIRE(TACEncoder64().GetAdditionalSize() == sizeof(uint32_t) + sizeof(byte_t));
		REQUIRE(TRangeEncoder32().GetAdditionalSize() == sizeof(uint64_t));
		REQUIRE(TRangeEncoder64().GetAdditionalSize() == sizeof(uint64_t));
	}
}

// ---------------------------------------------------------------------------
// Empty input
// ---------------------------------------------------------------------------

namespace
{
	template<class TPair>
	void EmptyInput()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		INFO(TPair::Name());

		const auto cum = MakeCumulative(std::vector<TCodeValue>{ 1, 1 });
		const std::vector<byte_t> bytes = EncodeStatic<TPair>(std::vector<size_t>(), cum);

		// EncodeFinish always flushes something, but no more than the declared overhead
		REQUIRE_FALSE(bytes.empty());
		REQUIRE(bytes.size() <= typename TPair::Encoder().GetAdditionalSize());

		// the decoder can start on it and simply has nothing to decode
		REQUIRE(DecodeStatic<TPair>(bytes, cum, 0).empty());
	}
}

TEST_CASE("Entropy coders handle an empty input", "[entropy][limits]")
{
	SECTION("TAC 32")    { EmptyInput<SAC32>(); }
	SECTION("TAC 64")    { EmptyInput<SAC64>(); }
	SECTION("TRange 32") { EmptyInput<SRC32>(); }
	SECTION("TRange 64") { EmptyInput<SRC64>(); }
}

// ---------------------------------------------------------------------------
// Total frequency right at MaxRange
// ---------------------------------------------------------------------------

namespace
{
	template<class TPair>
	void TotalAtMaxRange()
	{
		typedef typename TPair::Encoder Encoder;
		typedef CodeValueOf<Encoder> TCodeValue;
		INFO(TPair::Name());

		const TCodeValue total = Encoder::MaxRange;
		// three symbols: a tiny one at each end and a huge one in the middle
		const std::vector<TCodeValue> cum = { 0, 1, total - 1, total };

		std::vector<size_t> symbols;
		for (int i = 0; i < 500; ++i)
			symbols.push_back(size_t(i % 7 == 0 ? 0 : (i % 11 == 0 ? 2 : 1)));

		const std::vector<byte_t> bytes = EncodeStatic<TPair>(symbols, cum);
		REQUIRE(DecodeStatic<TPair>(bytes, cum, symbols.size()) == symbols);
	}

	template<class TPair>
	void RareSymbolsOnly()
	{
		typedef typename TPair::Encoder Encoder;
		typedef CodeValueOf<Encoder> TCodeValue;
		INFO(TPair::Name());

		const TCodeValue total = Encoder::MaxRange;
		const std::vector<TCodeValue> cum = { 0, 1, total - 1, total };

		// only the two probability-1/MaxRange symbols: worst case for precision
		std::vector<size_t> symbols;
		for (int i = 0; i < 64; ++i)
			symbols.push_back(size_t(i % 2 == 0 ? 0 : 2));

		const std::vector<byte_t> bytes = EncodeStatic<TPair>(symbols, cum);
		REQUIRE(DecodeStatic<TPair>(bytes, cum, symbols.size()) == symbols);
	}
}

TEST_CASE("Entropy coders work with the total frequency at MaxRange", "[entropy][limits]")
{
	SECTION("TAC 32")    { TotalAtMaxRange<SAC32>(); }
	SECTION("TAC 64")    { TotalAtMaxRange<SAC64>(); }
	SECTION("TRange 32") { TotalAtMaxRange<SRC32>(); }
	SECTION("TRange 64") { TotalAtMaxRange<SRC64>(); }
}

TEST_CASE("Entropy coders encode minimum-probability symbols at MaxRange", "[entropy][limits]")
{
	SECTION("TAC 32")    { RareSymbolsOnly<SAC32>(); }
	SECTION("TAC 64")    { RareSymbolsOnly<SAC64>(); }
	SECTION("TRange 32") { RareSymbolsOnly<SRC32>(); }
	SECTION("TRange 64") { RareSymbolsOnly<SRC64>(); }
}

// ---------------------------------------------------------------------------
// Output stream behaviour
// ---------------------------------------------------------------------------

namespace
{
	template<class TPair>
	void ReportsFullOutputStream()
	{
		typedef typename TPair::Encoder Encoder;
		typedef CodeValueOf<Encoder> TCodeValue;
		INFO(TPair::Name());

		const auto cum = MakeCumulative(std::vector<TCodeValue>(256, 1));
		const std::vector<size_t> symbols = RandomSymbols(std::vector<TCodeValue>(256, 1), 2000, 5);

		// a fixed 64-byte sink: ~8 bits per symbol means it fills up quickly
		std::vector<byte_t> sink(64, 0);
		CFxMemoryWriteStream ws;
		ws.AttachBuffer(sink.data(), sink.size());

		Encoder enc;
		enc.SetStream(&ws);

		bool failed = false;
		for (size_t s : symbols)
		{
			if (!enc.EncodeSymbol(cum[s], cum[s + 1], cum.back()))
			{
				failed = true;
				break;
			}
		}
		REQUIRE(failed);
		REQUIRE(ws.Pos() == sink.size()); // it filled the sink completely before giving up
	}

	template<class TPair>
	void FinishFailsOnFullStream()
	{
		typedef typename TPair::Encoder Encoder;
		INFO(TPair::Name());

		std::vector<byte_t> sink(1, 0);
		CFxMemoryWriteStream ws;
		ws.AttachBuffer(sink.data(), sink.size());
		ws.Seek(1, soFromBegin); // already full

		Encoder enc;
		enc.SetStream(&ws);
		REQUIRE_FALSE(enc.EncodeFinish());
	}

	template<class TPair>
	void AppendsToExistingStreamContent()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		INFO(TPair::Name());

		const auto cum = MakeCumulative(std::vector<TCodeValue>{ 3, 1, 4, 1, 5 });
		const std::vector<size_t> symbols = RandomSymbols(std::vector<TCodeValue>{ 3, 1, 4, 1, 5 }, 700, 8);

		// a header written by someone else precedes the coded data
		CWriteMemoryStream ws;
		ws.Write(uint32_t(0xCAFEBABE));
		ws.Write(std::string("header"));
		const size_t headerSize = ws.Pos();

		typename TPair::Encoder enc;
		enc.SetStream(&ws);
		for (size_t s : symbols)
			REQUIRE(enc.EncodeSymbol(cum[s], cum[s + 1], cum.back()));
		REQUIRE(enc.EncodeFinish());

		CReadMemoryStream rs;
		rs.AttachBuffer(ws.Buffer(), ws.Pos());
		REQUIRE(rs.ReadIntu32() == 0xCAFEBABE);
		REQUIRE(rs.ReadAstr() == "header");
		REQUIRE(rs.Pos() == headerSize);

		typename TPair::Decoder dec;
		dec.SetStream(&rs);
		dec.StartDecode();
		for (size_t i = 0; i < symbols.size(); ++i)
		{
			const size_t s = SymbolFromFreq(cum, dec.GetFreq(cum.back()));
			dec.DecodeSymbol(cum[s], cum[s + 1], cum.back());
			REQUIRE(s == symbols[i]);
		}
	}

	template<class TPair>
	void ReusableAfterSetStream()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		INFO(TPair::Name());

		const std::vector<TCodeValue> freqs = { 2, 2, 2, 2 };
		const auto cum = MakeCumulative(freqs);
		const std::vector<size_t> first = RandomSymbols(freqs, 300, 21);
		const std::vector<size_t> second = RandomSymbols(freqs, 300, 22);

		typename TPair::Encoder enc;
		CWriteMemoryStream ws1, ws2;

		enc.SetStream(&ws1);
		for (size_t s : first)
			REQUIRE(enc.EncodeSymbol(cum[s], cum[s + 1], cum.back()));
		REQUIRE(enc.EncodeFinish());

		enc.SetStream(&ws2); // resets the coder state
		for (size_t s : second)
			REQUIRE(enc.EncodeSymbol(cum[s], cum[s + 1], cum.back()));
		REQUIRE(enc.EncodeFinish());

		// the second stream must equal a fresh encoding of `second`
		REQUIRE(std::vector<byte_t>(ws2.Buffer(), ws2.Buffer() + ws2.Pos()) == EncodeStatic<TPair>(second, cum));

		typename TPair::Decoder dec;
		CReadMemoryStream rs1, rs2;
		rs1.AttachBuffer(ws1.Buffer(), ws1.Pos());
		rs2.AttachBuffer(ws2.Buffer(), ws2.Pos());

		dec.SetStream(&rs1);
		dec.StartDecode();
		for (size_t i = 0; i < first.size(); ++i)
		{
			const size_t s = SymbolFromFreq(cum, dec.GetFreq(cum.back()));
			dec.DecodeSymbol(cum[s], cum[s + 1], cum.back());
			REQUIRE(s == first[i]);
		}

		dec.SetStream(&rs2);
		dec.StartDecode();
		for (size_t i = 0; i < second.size(); ++i)
		{
			const size_t s = SymbolFromFreq(cum, dec.GetFreq(cum.back()));
			dec.DecodeSymbol(cum[s], cum[s + 1], cum.back());
			REQUIRE(s == second[i]);
		}
	}
}

TEST_CASE("Entropy encoders report a full output stream instead of throwing", "[entropy][stream]")
{
	SECTION("TAC 32")    { ReportsFullOutputStream<SAC32>(); }
	SECTION("TAC 64")    { ReportsFullOutputStream<SAC64>(); }
	SECTION("TRange 32") { ReportsFullOutputStream<SRC32>(); }
	SECTION("TRange 64") { ReportsFullOutputStream<SRC64>(); }
}

TEST_CASE("Entropy encoders fail EncodeFinish on a full output stream", "[entropy][stream]")
{
	SECTION("TAC 32")    { FinishFailsOnFullStream<SAC32>(); }
	SECTION("TAC 64")    { FinishFailsOnFullStream<SAC64>(); }
	SECTION("TRange 32") { FinishFailsOnFullStream<SRC32>(); }
	SECTION("TRange 64") { FinishFailsOnFullStream<SRC64>(); }
}

TEST_CASE("Entropy coders append to and read after existing stream content", "[entropy][stream]")
{
	SECTION("TAC 32")    { AppendsToExistingStreamContent<SAC32>(); }
	SECTION("TAC 64")    { AppendsToExistingStreamContent<SAC64>(); }
	SECTION("TRange 32") { AppendsToExistingStreamContent<SRC32>(); }
	SECTION("TRange 64") { AppendsToExistingStreamContent<SRC64>(); }
}

TEST_CASE("Entropy coders can be reused after SetStream", "[entropy][stream]")
{
	SECTION("TAC 32")    { ReusableAfterSetStream<SAC32>(); }
	SECTION("TAC 64")    { ReusableAfterSetStream<SAC64>(); }
	SECTION("TRange 32") { ReusableAfterSetStream<SRC32>(); }
	SECTION("TRange 64") { ReusableAfterSetStream<SRC64>(); }
}
