#include "TestCommon.h"
#include "CommonLib/stream/BitStream.h"
#include "CommonLib/stream/FixedBitStream.h"
#include "CommonLib/stream/WriteBitStream.h"

using namespace CommonLib;

namespace
{
	// Number of bytes a bit writer has touched so far: the current byte index
	// plus the byte being filled (there is always one once a bit was written).
	template<class TBitStream>
	size_t UsedBytes(const TBitStream& s)
	{
		return s.Pos() + 1;
	}
}

// ---------------------------------------------------------------------------
// CFxBitWriteStream / CFxBitReadStream - layout
// ---------------------------------------------------------------------------

TEST_CASE("CFxBitWriteStream packs bits LSB first", "[stream][bitstream][fx]")
{
	std::vector<byte_t> buf(4, 0);
	CFxBitWriteStream w;
	w.AttachBuffer(buf.data(), buf.size());

	SECTION("single bits")
	{
		w.WriteBit(true);
		for (int i = 0; i < 6; ++i)
			w.WriteBit(false);
		w.WriteBit(true);
		REQUIRE(buf[0] == 0x81);
		REQUIRE(UsedBytes(w) == 1);
	}

	SECTION("WriteBit(byte_t) only looks at the lowest bit")
	{
		w.WriteBit(byte_t(2)); // -> 0
		w.WriteBit(byte_t(3)); // -> 1
		w.WriteBit(byte_t(0)); // -> 0
		w.WriteBit(byte_t(1)); // -> 1
		REQUIRE(buf[0] == 0x0A);
	}

	SECTION("a full byte")
	{
		w.WriteBits(byte_t(0xA5), 8);
		REQUIRE(buf[0] == 0xA5);
		REQUIRE(UsedBytes(w) == 1);
	}

	SECTION("a uint16_t spans two bytes, low byte first")
	{
		w.WriteBits(uint16_t(0xBEEF), 16);
		REQUIRE(buf[0] == 0xEF);
		REQUIRE(buf[1] == 0xBE);
		REQUIRE(UsedBytes(w) == 2);
	}

	SECTION("partial widths pack back to back")
	{
		w.WriteBits(byte_t(0x05), 3);   // 101
		w.WriteBits(byte_t(0x03), 2);   // 11      -> bits 3,4
		w.WriteBits(uint16_t(0x1FF), 9); // 111111111 -> bits 5..13
		// byte0: bits 0-2 = 101, bits 3-4 = 11, bits 5-7 = 111 -> 1111 1101 = 0xFD
		// byte1: bits 0-5 = 111111 -> 0x3F
		REQUIRE(buf[0] == 0xFD);
		REQUIRE(buf[1] == 0x3F);
		REQUIRE(UsedBytes(w) == 2);
	}

	SECTION("WriteBits with zero count writes nothing")
	{
		w.WriteBits(uint32_t(0xFFFFFFFF), 0);
		REQUIRE(buf[0] == 0);
		REQUIRE(w.Pos() == 0);
	}
}

TEST_CASE("CFxBitReadStream unpacks bits LSB first", "[stream][bitstream][fx]")
{
	byte_t buf[3] = { 0x81, 0xEF, 0xBE };
	CFxBitReadStream r;
	r.AttachBuffer(buf, 3);

	REQUIRE(r.ReadBit() == true);
	for (int i = 0; i < 6; ++i)
		REQUIRE(r.ReadBit() == false);
	REQUIRE(r.ReadBit() == true);

	uint16_t v = 0;
	r.ReadBits(v, 16);
	REQUIRE(v == 0xBEEF);
}

// ---------------------------------------------------------------------------
// Round trips
// ---------------------------------------------------------------------------

TEST_CASE("Fixed bit stream mixed-width round trip", "[stream][bitstream][fx][roundtrip]")
{
	std::vector<byte_t> buf(64, 0); // 260 bits -> 33 bytes needed

	const byte_t   v3  = 0x05;                 // 3 bits
	const bool     b1  = true;                 // 1 bit
	const uint16_t v13 = 0x1ABC & 0x1FFF;      // 13 bits
	const uint32_t v20 = 0xABCDE;              // 20 bits
	const uint64_t v64 = 0x8123456789ABCDEFULL;// 64 bits
	const byte_t   v7  = 0x55;                 // 7 bits
	const byte_t   v8  = 0xC3;                 // 8 bits
	const uint32_t v32 = 0xFFFFFFFF;           // 32 bits
	const int16_t  s16 = -5;                   // 16 bits, signed
	const int32_t  s32 = -123456;              // 32 bits, signed
	const int64_t  s64 = -1234567890123LL;     // 64 bits, signed
	const size_t totalBits = 3 + 1 + 13 + 20 + 64 + 7 + 8 + 32 + 16 + 32 + 64;

	{
		CFxBitWriteStream w;
		w.AttachBuffer(buf.data(), buf.size());
		w.WriteBits(v3, 3);
		w.WriteBit(b1);
		w.WriteBits(v13, 13);
		w.WriteBits(v20, 20);
		w.WriteBits(v64, 64);
		w.WriteBits(v7, 7);
		w.WriteBits(v8, 8);
		w.WriteBits(v32, 32);
		w.WriteBits(s16, 16);
		w.WriteBits(s32, 32);
		w.WriteBits(s64, 64);
		REQUIRE(UsedBytes(w) == (totalBits + 7) / 8);
	}

	CFxBitReadStream r;
	r.AttachBuffer(buf.data(), buf.size());

	byte_t r3 = 0; r.ReadBits(r3, 3);            REQUIRE(r3 == v3);
	REQUIRE(r.ReadBit() == b1);
	uint16_t r13 = 0; r.ReadBits(r13, 13);       REQUIRE(r13 == v13);
	uint32_t r20 = 0; r.ReadBits(r20, 20);       REQUIRE(r20 == v20);
	uint64_t r64 = 0; r.ReadBits(r64, 64);       REQUIRE(r64 == v64);
	byte_t r7 = 0; r.ReadBits(r7, 7);            REQUIRE(r7 == v7);
	byte_t r8 = 0; r.ReadBits(r8, 8);            REQUIRE(r8 == v8);
	uint32_t r32 = 0; r.ReadBits(r32, 32);       REQUIRE(r32 == v32);
	int16_t rs16 = 0; r.ReadBits(rs16, 16);      REQUIRE(rs16 == s16);
	int32_t rs32 = 0; r.ReadBits(rs32, 32);      REQUIRE(rs32 == s32);
	int64_t rs64 = 0; r.ReadBits(rs64, 64);      REQUIRE(rs64 == s64);
}

TEST_CASE("Fixed bit stream truncates to the requested width", "[stream][bitstream][fx]")
{
	std::vector<byte_t> buf(4, 0);
	CFxBitWriteStream w;
	w.AttachBuffer(buf.data(), buf.size());

	w.WriteBits(uint32_t(0xFFFFFFFF), 5); // only the low 5 bits
	REQUIRE(buf[0] == 0x1F);
	REQUIRE(buf[1] == 0);

	CFxBitReadStream r;
	r.AttachBuffer(buf.data(), buf.size());
	uint32_t v = 0xDEADBEEF;
	r.ReadBits(v, 5);
	REQUIRE(v == 0x1F); // the target is fully replaced, not OR-ed into
}

TEST_CASE("Fixed bit stream WriteBitsSafe round trip", "[stream][bitstream][fx][safe]")
{
	std::vector<byte_t> buf(16, 0); // 65 bits -> 9 bytes needed
	CFxBitWriteStream w;
	w.AttachBuffer(buf.data(), buf.size());

	REQUIRE(w.WriteBitSafe(true));
	REQUIRE(w.WriteBitSafe(byte_t(0)));
	REQUIRE(w.WriteBitsSafe(byte_t(0x7F), 7));
	REQUIRE(w.WriteBitsSafe(uint16_t(0x1234), 16));
	REQUIRE(w.WriteBitsSafe(uint32_t(0x89ABCDEF), 32));
	REQUIRE(w.WriteBitsSafe(int16_t(-2), 8)); // low 8 bits of -2 -> 0xFE

	CFxBitReadStream r;
	r.AttachBuffer(buf.data(), buf.size());
	REQUIRE(r.ReadBit() == true);
	REQUIRE(r.ReadBit() == false);
	byte_t b = 0; r.ReadBits(b, 7);         REQUIRE(b == 0x7F);
	uint16_t u16 = 0; r.ReadBits(u16, 16);  REQUIRE(u16 == 0x1234);
	uint32_t u32 = 0; r.ReadBits(u32, 32);  REQUIRE(u32 == 0x89ABCDEF);
	byte_t low = 0; r.ReadBits(low, 8);     REQUIRE(low == 0xFE);
}

// ---------------------------------------------------------------------------
// Capacity limits. The vectors carry one extra "guard" byte that is NOT
// attached to the stream, so an off-by-one write is caught by the checks
// below instead of corrupting the heap.
// ---------------------------------------------------------------------------

TEST_CASE("CFxBitWriteStream accepts exactly 8*N bits into N bytes", "[stream][bitstream][fx][bounds]")
{
	const size_t N = 3;
	std::vector<byte_t> buf(N + 1, 0);
	CFxBitWriteStream w;
	w.AttachBuffer(buf.data(), N);

	for (size_t i = 0; i < 8 * N; ++i)
		REQUIRE(w.WriteBitSafe(true));

	REQUIRE(UsedBytes(w) == N);
	for (size_t i = 0; i < N; ++i)
		REQUIRE(buf[i] == 0xFF);
	REQUIRE(buf[N] == 0); // guard untouched
}

TEST_CASE("CFxBitWriteStream rejects the bit after the last byte", "[stream][bitstream][fx][bounds]")
{
	const size_t N = 2;

	SECTION("WriteBit throws")
	{
		std::vector<byte_t> buf(N + 1, 0xAA);
		CFxBitWriteStream w;
		w.AttachBuffer(buf.data(), N);
		for (size_t i = 0; i < 8 * N; ++i)
			w.WriteBit(true);

		REQUIRE_THROWS_AS(w.WriteBit(true), CExcBase);
		REQUIRE_THROWS_AS(w.WriteBit(true), CExcBase); // stays failed
		REQUIRE(buf[N] == 0xAA); // guard byte must not be touched
		REQUIRE(buf[0] == 0xFF);
		REQUIRE(buf[1] == 0xFF);
	}

	SECTION("WriteBitSafe returns false")
	{
		std::vector<byte_t> buf(N + 1, 0xAA);
		CFxBitWriteStream w;
		w.AttachBuffer(buf.data(), N);
		for (size_t i = 0; i < 8 * N; ++i)
			REQUIRE(w.WriteBitSafe(true));

		REQUIRE_FALSE(w.WriteBitSafe(true));
		REQUIRE(buf[N] == 0xAA);
	}

	SECTION("WriteBitsSafe reports the overflow")
	{
		std::vector<byte_t> buf(N + 1, 0xAA);
		CFxBitWriteStream w;
		w.AttachBuffer(buf.data(), N);
		REQUIRE(w.WriteBitsSafe(uint16_t(0xFFFF), 16));
		REQUIRE_FALSE(w.WriteBitsSafe(byte_t(1), 1));
		REQUIRE(buf[N] == 0xAA);
	}

	SECTION("an empty stream accepts nothing")
	{
		CFxBitWriteStream w;
		REQUIRE_THROWS_AS(w.WriteBit(true), CExcBase);
		REQUIRE_FALSE(w.WriteBitSafe(true));
	}
}

TEST_CASE("CFxBitReadStream reads exactly 8*N bits from N bytes", "[stream][bitstream][fx][bounds]")
{
	const size_t N = 3;
	std::vector<byte_t> buf(N + 1, 0xFF);
	CFxBitReadStream r;
	r.AttachBuffer(buf.data(), N);

	for (size_t i = 0; i < 8 * N; ++i)
		REQUIRE(r.ReadBit() == true);
}

TEST_CASE("CFxBitReadStream throws on the bit after the last byte", "[stream][bitstream][fx][bounds]")
{
	const size_t N = 2;
	std::vector<byte_t> buf(N + 1, 0xFF);
	CFxBitReadStream r;
	r.AttachBuffer(buf.data(), N);

	for (size_t i = 0; i < 8 * N; ++i)
		r.ReadBit();

	REQUIRE_THROWS_AS(r.ReadBit(), CExcBase);
	REQUIRE_THROWS_AS(r.ReadBit(), CExcBase);

	SECTION("an empty stream has no bits")
	{
		CFxBitReadStream empty;
		REQUIRE_THROWS_AS(empty.ReadBit(), CExcBase);
	}
}

TEST_CASE("Bit stream Seek and Reset land on a byte boundary", "[stream][bitstream][fx][seek]")
{
	std::vector<byte_t> buf(4, 0);

	CFxBitWriteStream w;
	w.AttachBuffer(buf.data(), buf.size());
	w.WriteBits(byte_t(0x07), 3);      // byte 0 partially filled
	w.Seek(2, soFromBegin);            // jump to byte 2, bit cursor restarts at 0
	REQUIRE(w.Pos() == 2);
	w.WriteBits(byte_t(0xA5), 8);
	REQUIRE(buf[0] == 0x07);
	REQUIRE(buf[1] == 0x00);
	REQUIRE(buf[2] == 0xA5);

	w.Reset();
	REQUIRE(w.Pos() == 0);
	w.WriteBits(byte_t(0x18), 5);      // ORs into byte 0 from bit 0 again
	REQUIRE(buf[0] == 0x1F);

	CFxBitReadStream r;
	r.AttachBuffer(buf.data(), buf.size());
	REQUIRE(r.ReadBit() == true);
	r.Seek(2, soFromBegin);
	byte_t v = 0;
	r.ReadBits(v, 8);
	REQUIRE(v == 0xA5);
	r.Seek(1, soFromCurrent);          // byte 3 -> past its end after 8 bits
	REQUIRE(r.Pos() == 4);
	REQUIRE_THROWS_AS(r.ReadBit(), CExcBase);
	r.Reset();
	r.ReadBits(v, 8);
	REQUIRE(v == 0x1F);
}

// ---------------------------------------------------------------------------
// WriteBitStream (growing)
// ---------------------------------------------------------------------------

namespace
{
	void PrepareGrowingWriter(WriteBitStream& w, size_t initialBytes)
	{
		w.Create(initialBytes);
	}
}

TEST_CASE("WriteBitStream Create yields a zeroed buffer", "[stream][bitstream][grow]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	WriteBitStream w(alloc);
	w.Create(64);
	REQUIRE(w.Size() == 64);
	REQUIRE(w.Pos() == 0);
	for (size_t i = 0; i < w.Size(); ++i)
		REQUIRE(w.Buffer()[i] == 0);

	w.WriteBits(byte_t(0x01), 1);
	REQUIRE(w.Buffer()[0] == 0x01); // nothing but our bit is set
}

TEST_CASE("WriteBitStream works without an explicit Create", "[stream][bitstream][grow]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	WriteBitStream w(alloc);
	REQUIRE(w.Size() == 0);

	w.WriteBits(uint16_t(0xBEEF), 16);
	REQUIRE(UsedBytes(w) == 2);
	REQUIRE(w.Buffer()[0] == 0xEF);
	REQUIRE(w.Buffer()[1] == 0xBE);
}

TEST_CASE("WriteBitStream WriteBitSafe", "[stream][bitstream][grow][safe]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);

	SECTION("succeeds while it can grow")
	{
		WriteBitStream w(alloc);
		for (int i = 0; i < 100; ++i)
			REQUIRE(w.WriteBitSafe(i % 3 == 0));
		REQUIRE(w.WriteBitsSafe(uint32_t(0xDEADBEEF), 32));
		REQUIRE(UsedBytes(w) == (100 + 32 + 7) / 8);

		CFxBitReadStream r;
		r.AttachBuffer(w.Buffer(), UsedBytes(w));
		for (int i = 0; i < 100; ++i)
			REQUIRE(r.ReadBit() == (i % 3 == 0));
		uint32_t v = 0;
		r.ReadBits(v, 32);
		REQUIRE(v == 0xDEADBEEF);
	}

	SECTION("returns false when an attached buffer is full")
	{
		std::vector<byte_t> external(1 + 1, 0);
		WriteBitStream w(alloc);
		w.AttachBuffer(external.data(), 1);
		for (int i = 0; i < 8; ++i)
			REQUIRE(w.WriteBitSafe(true));
		REQUIRE(w.Pos() == 0);

		// every refused write must leave the stream where it was, so repeated
		// attempts can never walk past the end of the buffer
		for (int attempt = 0; attempt < 5; ++attempt)
		{
			REQUIRE_FALSE(w.WriteBitSafe(true));
			REQUIRE_FALSE(w.WriteBitsSafe(byte_t(0xFF), 8));
			REQUIRE_THROWS_AS(w.WriteBit(true), std::exception);
			REQUIRE(w.Pos() == 0);
		}
		REQUIRE(external[0] == 0xFF);
		REQUIRE(external[1] == 0); // guard byte untouched
	}
}

TEST_CASE("WriteBitStream grows while writing and preserves earlier bits", "[stream][bitstream][grow][roundtrip]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	const size_t bitCount = 1000;

	{
		WriteBitStream w(alloc);
		PrepareGrowingWriter(w, 1);

		// pseudo-random but reproducible bit sequence
		for (size_t i = 0; i < bitCount; ++i)
			w.WriteBit(((i * 7 + 3) % 5) < 2);

		REQUIRE(UsedBytes(w) == (bitCount + 7) / 8);
		REQUIRE(w.Size() >= UsedBytes(w));
		REQUIRE(alloc->GetAllocCount() > 1); // it really had to grow

		CFxBitReadStream r;
		r.AttachBuffer(w.Buffer(), UsedBytes(w));
		for (size_t i = 0; i < bitCount; ++i)
			REQUIRE(r.ReadBit() == (((i * 7 + 3) % 5) < 2));
	}

	REQUIRE(alloc->GetCurrentMemoryBalance() == 0);
}

TEST_CASE("WriteBitStream multi-bit writes round trip across growth", "[stream][bitstream][grow][roundtrip]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	WriteBitStream w(alloc);
	PrepareGrowingWriter(w, 2);

	for (uint32_t i = 0; i < 300; ++i)
	{
		w.WriteBits(uint32_t(i * 2654435761u), 17);
		w.WriteBits(uint16_t(i), 9);
	}
	const size_t totalBits = 300 * (17 + 9);
	REQUIRE(UsedBytes(w) == (totalBits + 7) / 8);

	CFxBitReadStream r;
	r.AttachBuffer(w.Buffer(), UsedBytes(w));
	for (uint32_t i = 0; i < 300; ++i)
	{
		uint32_t a = 0; r.ReadBits(a, 17);
		uint16_t b = 0; r.ReadBits(b, 9);
		REQUIRE(a == (uint32_t(i * 2654435761u) & 0x1FFFF));
		REQUIRE(b == (uint16_t(i) & 0x1FF));
	}
}

TEST_CASE("WriteBitStream Resize grows to a zero-filled buffer", "[stream][bitstream][grow][resize]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	WriteBitStream w(alloc);
	PrepareGrowingWriter(w, 2);
	w.WriteBits(uint16_t(0x0FFF), 12); // byte 0 = 0xFF (complete), byte 1 = 0x0F (being filled)

	w.Resize(uint32_t(100));
	REQUIRE(w.Size() >= 101);
	REQUIRE(w.Pos() == 1);
	REQUIRE(w.Buffer()[0] == 0xFF);
	REQUIRE(w.Buffer()[1] == 0x0F);
	for (size_t i = 2; i < w.Size(); ++i)
		REQUIRE(w.Buffer()[i] == 0); // new space is zeroed
	REQUIRE(UsedBytes(w) == 2);
}

TEST_CASE("WriteBitStream Resize keeps the partially written byte", "[stream][bitstream][grow][resize]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	WriteBitStream w(alloc);
	PrepareGrowingWriter(w, 4);
	w.WriteBits(uint16_t(0x0FFF), 12); // byte 0 = 0xFF, byte 1 = 0x0F (half full)

	w.Resize(uint32_t(64));
	REQUIRE(w.Size() >= 65);
	REQUIRE(w.Buffer()[0] == 0xFF);
	REQUIRE(w.Buffer()[1] == 0x0F);

	w.WriteBits(byte_t(0x0F), 4); // completes byte 1
	REQUIRE(w.Buffer()[1] == 0xFF);
	REQUIRE(UsedBytes(w) == 2);
}

TEST_CASE("WriteBitStream on an attached buffer cannot grow", "[stream][bitstream][grow][attach]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	std::vector<byte_t> external(2 + 1, 0); // + guard
	WriteBitStream w(alloc);
	w.AttachBuffer(external.data(), 2);

	for (int i = 0; i < 16; ++i)
		w.WriteBit(true);
	REQUIRE(external[0] == 0xFF);
	REQUIRE(external[1] == 0xFF);

	REQUIRE_THROWS_AS(w.WriteBit(true), std::exception);
	REQUIRE_THROWS_AS(w.Resize(uint32_t(10)), std::exception);
	REQUIRE(external[2] == 0); // guard untouched
	REQUIRE(alloc->GetAllocCount() == 0);
}
