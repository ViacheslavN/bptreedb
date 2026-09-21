#include "TestCommon.h"

using namespace CommonLib;
using namespace test_utils;

// ---------------------------------------------------------------------------
// Round trips with a static model, for all four coder pairs
// ---------------------------------------------------------------------------

namespace
{
	template<class TPair>
	void RoundTrip(const std::vector<size_t>& symbols, const std::vector<CodeValueOf<typename TPair::Encoder>>& cum)
	{
		INFO(TPair::Name());
		const std::vector<byte_t> bytes = EncodeStatic<TPair>(symbols, cum);
		const std::vector<size_t> back = DecodeStatic<TPair>(bytes, cum, symbols.size());
		REQUIRE(back == symbols);
	}

	template<class TPair>
	void UniformAlphabet()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		const std::vector<TCodeValue> freqs(256, 1);
		const auto cum = MakeCumulative(freqs);
		RoundTrip<TPair>(RandomSymbols(freqs, 5000, 12345), cum);
	}

	template<class TPair>
	void SkewedAlphabet()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		const std::vector<TCodeValue> freqs = { 900, 50, 30, 15, 4, 1 };
		const auto cum = MakeCumulative(freqs);
		RoundTrip<TPair>(RandomSymbols(freqs, 8000, 777), cum);
	}

	template<class TPair>
	void BinaryAlphabet()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		const std::vector<TCodeValue> freqs = { 1, 1000 };
		const auto cum = MakeCumulative(freqs);

		// mostly symbol 1, with symbol 0 every 250th position
		std::vector<size_t> symbols(10000, 1);
		for (size_t i = 0; i < symbols.size(); i += 250)
			symbols[i] = 0;
		RoundTrip<TPair>(symbols, cum);
	}

	template<class TPair>
	void EverySymbolInOrder()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		std::vector<TCodeValue> freqs(64);
		for (size_t i = 0; i < freqs.size(); ++i)
			freqs[i] = TCodeValue(i + 1); // non-uniform, all non-zero
		const auto cum = MakeCumulative(freqs);

		std::vector<size_t> symbols;
		for (int pass = 0; pass < 3; ++pass)
			for (size_t s = 0; s < freqs.size(); ++s)
				symbols.push_back(s);
		for (size_t s = freqs.size(); s-- > 0;)
			symbols.push_back(s);
		RoundTrip<TPair>(symbols, cum);
	}

	template<class TPair>
	void SingleSymbol()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		const std::vector<TCodeValue> freqs = { 3, 5, 7 };
		const auto cum = MakeCumulative(freqs);
		RoundTrip<TPair>(std::vector<size_t>{ 0 }, cum);
		RoundTrip<TPair>(std::vector<size_t>{ 1 }, cum);
		RoundTrip<TPair>(std::vector<size_t>{ 2 }, cum);
	}

	template<class TPair>
	void ConstantRun()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		const std::vector<TCodeValue> freqs = { 1, 1, 1, 1 };
		const auto cum = MakeCumulative(freqs);
		RoundTrip<TPair>(std::vector<size_t>(3000, 2), cum);
		RoundTrip<TPair>(std::vector<size_t>(3000, 0), cum);
		RoundTrip<TPair>(std::vector<size_t>(3000, 3), cum);
	}
}

TEST_CASE("Entropy coders round-trip a uniform 256-symbol alphabet", "[entropy][roundtrip]")
{
	SECTION("TAC 32")    { UniformAlphabet<SAC32>(); }
	SECTION("TAC 64")    { UniformAlphabet<SAC64>(); }
	SECTION("TRange 32") { UniformAlphabet<SRC32>(); }
	SECTION("TRange 64") { UniformAlphabet<SRC64>(); }
}

TEST_CASE("Entropy coders round-trip a skewed alphabet", "[entropy][roundtrip]")
{
	SECTION("TAC 32")    { SkewedAlphabet<SAC32>(); }
	SECTION("TAC 64")    { SkewedAlphabet<SAC64>(); }
	SECTION("TRange 32") { SkewedAlphabet<SRC32>(); }
	SECTION("TRange 64") { SkewedAlphabet<SRC64>(); }
}

TEST_CASE("Entropy coders round-trip a heavily skewed binary alphabet", "[entropy][roundtrip]")
{
	SECTION("TAC 32")    { BinaryAlphabet<SAC32>(); }
	SECTION("TAC 64")    { BinaryAlphabet<SAC64>(); }
	SECTION("TRange 32") { BinaryAlphabet<SRC32>(); }
	SECTION("TRange 64") { BinaryAlphabet<SRC64>(); }
}

TEST_CASE("Entropy coders round-trip every symbol including the first and last", "[entropy][roundtrip]")
{
	SECTION("TAC 32")    { EverySymbolInOrder<SAC32>(); }
	SECTION("TAC 64")    { EverySymbolInOrder<SAC64>(); }
	SECTION("TRange 32") { EverySymbolInOrder<SRC32>(); }
	SECTION("TRange 64") { EverySymbolInOrder<SRC64>(); }
}

TEST_CASE("Entropy coders round-trip a single symbol", "[entropy][roundtrip]")
{
	SECTION("TAC 32")    { SingleSymbol<SAC32>(); }
	SECTION("TAC 64")    { SingleSymbol<SAC64>(); }
	SECTION("TRange 32") { SingleSymbol<SRC32>(); }
	SECTION("TRange 64") { SingleSymbol<SRC64>(); }
}

TEST_CASE("Entropy coders round-trip long constant runs", "[entropy][roundtrip]")
{
	SECTION("TAC 32")    { ConstantRun<SAC32>(); }
	SECTION("TAC 64")    { ConstantRun<SAC64>(); }
	SECTION("TRange 32") { ConstantRun<SRC32>(); }
	SECTION("TRange 64") { ConstantRun<SRC64>(); }
}

// ---------------------------------------------------------------------------
// Adaptive model: both sides update the frequencies as they go
// ---------------------------------------------------------------------------

namespace
{
	template<class TPair>
	void AdaptiveRoundTrip()
	{
		typedef typename TPair::Encoder Encoder;
		typedef typename TPair::Decoder Decoder;
		typedef CodeValueOf<Encoder> TCodeValue;
		INFO(TPair::Name());

		const size_t alphabet = 16;
		const size_t count = 6000; // total frequency stays far below every coder's MaxRange
		std::vector<TCodeValue> freqs(alphabet, 1);
		const std::vector<size_t> symbols = RandomSymbols(std::vector<TCodeValue>{ 40, 20, 10, 5, 5, 5, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1 }, count, 4242);

		CWriteMemoryStream ws;
		{
			Encoder enc;
			enc.SetStream(&ws);
			std::vector<TCodeValue> model = freqs;
			for (size_t s : symbols)
			{
				const auto cum = MakeCumulative(model);
				REQUIRE(enc.EncodeSymbol(cum[s], cum[s + 1], cum.back()));
				model[s]++;
			}
			REQUIRE(enc.EncodeFinish());
		}

		CReadMemoryStream rs;
		rs.AttachBuffer(ws.Buffer(), ws.Pos());
		Decoder dec;
		dec.SetStream(&rs);
		dec.StartDecode();

		std::vector<TCodeValue> model = freqs;
		for (size_t i = 0; i < count; ++i)
		{
			const auto cum = MakeCumulative(model);
			const size_t s = SymbolFromFreq(cum, dec.GetFreq(cum.back()));
			dec.DecodeSymbol(cum[s], cum[s + 1], cum.back());
			REQUIRE(s == symbols[i]);
			model[s]++;
		}
	}
}

TEST_CASE("Entropy coders work with an adaptive model", "[entropy][adaptive]")
{
	SECTION("TAC 32")    { AdaptiveRoundTrip<SAC32>(); }
	SECTION("TAC 64")    { AdaptiveRoundTrip<SAC64>(); }
	SECTION("TRange 32") { AdaptiveRoundTrip<SRC32>(); }
	SECTION("TRange 64") { AdaptiveRoundTrip<SRC64>(); }
}

// ---------------------------------------------------------------------------
// Compression behaviour
// ---------------------------------------------------------------------------

namespace
{
	template<class TPair>
	void CompressesSkewedInput()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		INFO(TPair::Name());

		const std::vector<TCodeValue> freqs = { 1, 1000 };
		const auto cum = MakeCumulative(freqs);
		std::vector<size_t> symbols(20000, 1);
		for (size_t i = 0; i < symbols.size(); i += 1000)
			symbols[i] = 0;

		// entropy is ~0.011 bit/symbol -> a few dozen bytes; anything under 1/50 of the raw size is fine
		const std::vector<byte_t> bytes = EncodeStatic<TPair>(symbols, cum);
		REQUIRE(bytes.size() < symbols.size() / 50);
		REQUIRE(DecodeStatic<TPair>(bytes, cum, symbols.size()) == symbols);
	}

	template<class TPair>
	void UniformInputIsNotInflated()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		INFO(TPair::Name());

		const std::vector<TCodeValue> freqs(256, 1);
		const auto cum = MakeCumulative(freqs);
		const std::vector<size_t> symbols = RandomSymbols(freqs, 4096, 99);

		// 8 bits per symbol of information: output must be within a few flush bytes of the input size
		const std::vector<byte_t> bytes = EncodeStatic<TPair>(symbols, cum);
		REQUIRE(bytes.size() >= symbols.size() - 4);
		REQUIRE(bytes.size() <= symbols.size() + 16);
	}

	template<class TPair>
	void Deterministic()
	{
		typedef CodeValueOf<typename TPair::Encoder> TCodeValue;
		INFO(TPair::Name());

		const std::vector<TCodeValue> freqs = { 5, 3, 2, 9 };
		const auto cum = MakeCumulative(freqs);
		const std::vector<size_t> symbols = RandomSymbols(freqs, 1000, 1);

		REQUIRE(EncodeStatic<TPair>(symbols, cum) == EncodeStatic<TPair>(symbols, cum));

		std::vector<size_t> changed = symbols;
		changed[500] = (changed[500] + 1) % freqs.size();
		REQUIRE(EncodeStatic<TPair>(changed, cum) != EncodeStatic<TPair>(symbols, cum));
	}
}

TEST_CASE("Entropy coders compress a heavily skewed input", "[entropy][compression]")
{
	SECTION("TAC 32")    { CompressesSkewedInput<SAC32>(); }
	SECTION("TAC 64")    { CompressesSkewedInput<SAC64>(); }
	SECTION("TRange 32") { CompressesSkewedInput<SRC32>(); }
	SECTION("TRange 64") { CompressesSkewedInput<SRC64>(); }
}

TEST_CASE("Entropy coders do not inflate incompressible input", "[entropy][compression]")
{
	SECTION("TAC 32")    { UniformInputIsNotInflated<SAC32>(); }
	SECTION("TAC 64")    { UniformInputIsNotInflated<SAC64>(); }
	SECTION("TRange 32") { UniformInputIsNotInflated<SRC32>(); }
	SECTION("TRange 64") { UniformInputIsNotInflated<SRC64>(); }
}

TEST_CASE("Entropy coders are deterministic and input-sensitive", "[entropy][compression]")
{
	SECTION("TAC 32")    { Deterministic<SAC32>(); }
	SECTION("TAC 64")    { Deterministic<SAC64>(); }
	SECTION("TRange 32") { Deterministic<SRC32>(); }
	SECTION("TRange 64") { Deterministic<SRC64>(); }
}
