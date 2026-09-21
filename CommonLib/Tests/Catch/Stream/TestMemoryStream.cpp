#include "TestCommon.h"

using namespace CommonLib;
using test_utils::MakePattern;
using test_utils::SameBytes;
using test_utils::AttachWritten;

// ---------------------------------------------------------------------------
// IStream
// ---------------------------------------------------------------------------

TEST_CASE("IStream::IsBigEndian matches the host", "[stream]")
{
	REQUIRE(CommonLib::IStream::IsBigEndian() == test_utils::HostIsBigEndian());
}

// ---------------------------------------------------------------------------
// CWriteMemoryStream - basic growth and byte-level behaviour
// ---------------------------------------------------------------------------

TEST_CASE("CWriteMemoryStream initial state", "[stream][memstream][write]")
{
	CWriteMemoryStream ws;
	REQUIRE(ws.Pos() == 0);
	REQUIRE(ws.Size() == 0);
	REQUIRE(ws.Buffer() == nullptr);
	REQUIRE(ws.IsEnoughSpace(std::numeric_limits<uint32_t>::max()));
}

TEST_CASE("CWriteMemoryStream WriteBytes grows the buffer and stores data", "[stream][memstream][write]")
{
	CWriteMemoryStream ws;
	const std::vector<byte_t> data = MakePattern(100);

	REQUIRE(ws.WriteBytes(data.data(), data.size()) == (std::streamsize)data.size());

	REQUIRE(ws.Pos() == 100);
	REQUIRE(ws.Size() >= 100); // Size() is the capacity, never smaller than what was written
	REQUIRE(SameBytes(ws.Buffer(), data));
	REQUIRE(ws.BufferFromCurPos() == ws.Buffer() + ws.Pos());
}

TEST_CASE("CWriteMemoryStream WriteBytes with size 0 is a no-op", "[stream][memstream][write]")
{
	CWriteMemoryStream ws;
	byte_t dummy = 0;
	REQUIRE(ws.WriteBytes(&dummy, 0) == 0);
	REQUIRE(ws.Pos() == 0);
	REQUIRE(ws.Size() == 0);
}

TEST_CASE("CWriteMemoryStream keeps data across many reallocations", "[stream][memstream][write]")
{
	CWriteMemoryStream ws;
	const std::vector<byte_t> data = MakePattern(5000, 11);

	// one byte at a time forces every capacity step
	for (byte_t b : data)
		ws.Write(b);

	REQUIRE(ws.Pos() == data.size());
	REQUIRE(ws.Size() >= data.size());
	REQUIRE(SameBytes(ws.Buffer(), data));
}

TEST_CASE("CWriteMemoryStream concatenates several writes", "[stream][memstream][write]")
{
	CWriteMemoryStream ws;
	const std::vector<byte_t> a = MakePattern(13, 1);
	const std::vector<byte_t> b = MakePattern(77, 2);
	const std::vector<byte_t> c = MakePattern(1, 3);

	ws.Write(a.data(), a.size());
	ws.Write(b.data(), b.size());
	ws.Write(c.data(), c.size());

	std::vector<byte_t> expected;
	expected.insert(expected.end(), a.begin(), a.end());
	expected.insert(expected.end(), b.begin(), b.end());
	expected.insert(expected.end(), c.begin(), c.end());

	REQUIRE(ws.Pos() == expected.size());
	REQUIRE(SameBytes(ws.Buffer(), expected));
}

// ---------------------------------------------------------------------------
// Round trips: CWriteMemoryStream -> CReadMemoryStream
// ---------------------------------------------------------------------------

TEST_CASE("Memory stream scalar round trip via Write/Read overloads", "[stream][memstream][roundtrip]")
{
	CWriteMemoryStream ws;

	ws.Write(true);
	ws.Write(false);
	ws.Write(uint8_t(0xAB));
	ws.Write(char('z'));
	ws.Write(int16_t(-1234));
	ws.Write(uint16_t(54321));
	ws.Write(int32_t(-123456789));
	ws.Write(uint32_t(0xDEADBEEF));
	ws.Write(int64_t(-1234567890123456789LL));
	ws.Write(uint64_t(0xFEDCBA9876543210ULL));
	ws.Write(3.5f);
	ws.Write(-2.25);

	const size_t expectedBytes = 1 + 1 + 1 + 1 + 2 + 2 + 4 + 4 + 8 + 8 + 4 + 8;
	REQUIRE(ws.Pos() == expectedBytes);

	CReadMemoryStream rs;
	AttachWritten(rs, ws);
	REQUIRE(rs.Size() == expectedBytes);
	REQUIRE(rs.Pos() == 0);

	SECTION("Read(T&) overloads")
	{
		bool b1 = false, b2 = true;
		uint8_t u8 = 0; char ch = 0;
		int16_t i16 = 0; uint16_t u16 = 0;
		int32_t i32 = 0; uint32_t u32 = 0;
		int64_t i64 = 0; uint64_t u64 = 0;
		float f = 0; double d = 0;

		rs.Read(b1); rs.Read(b2);
		rs.Read(u8); rs.Read(ch);
		rs.Read(i16); rs.Read(u16);
		rs.Read(i32); rs.Read(u32);
		rs.Read(i64); rs.Read(u64);
		rs.Read(f); rs.Read(d);

		REQUIRE(b1 == true);
		REQUIRE(b2 == false);
		REQUIRE(u8 == 0xAB);
		REQUIRE(ch == 'z');
		REQUIRE(i16 == -1234);
		REQUIRE(u16 == 54321);
		REQUIRE(i32 == -123456789);
		REQUIRE(u32 == 0xDEADBEEF);
		REQUIRE(i64 == -1234567890123456789LL);
		REQUIRE(u64 == 0xFEDCBA9876543210ULL);
		REQUIRE(f == 3.5f);
		REQUIRE(d == -2.25);
		REQUIRE(rs.Pos() == expectedBytes);
	}

	SECTION("ReadXxx() getters")
	{
		REQUIRE(rs.ReadBool() == true);
		REQUIRE(rs.ReadBool() == false);
		REQUIRE(rs.ReadByte() == 0xAB);
		REQUIRE(rs.ReadChar() == 'z');
		REQUIRE(rs.Readint16() == -1234);
		REQUIRE(rs.Readintu16() == 54321);
		REQUIRE(rs.ReadInt32() == -123456789);
		REQUIRE(rs.ReadIntu32() == 0xDEADBEEF);
		REQUIRE(rs.ReadInt64() == -1234567890123456789LL);
		REQUIRE(rs.ReadIntu64() == 0xFEDCBA9876543210ULL);
		REQUIRE(rs.ReadFloat() == 3.5f);
		REQUIRE(rs.ReadDouble() == -2.25);
		REQUIRE(rs.Pos() == expectedBytes);
	}

	SECTION("ReadDword is an alias for a uint32_t read")
	{
		rs.Seek(1 + 1 + 1 + 1 + 2 + 2 + 4, soFromBegin);
		REQUIRE(rs.ReadDword() == 0xDEADBEEF);
	}
}

TEST_CASE("Memory stream extreme scalar values round trip", "[stream][memstream][roundtrip]")
{
	CWriteMemoryStream ws;
	ws.Write(std::numeric_limits<int16_t>::min());
	ws.Write(std::numeric_limits<int16_t>::max());
	ws.Write(std::numeric_limits<uint16_t>::max());
	ws.Write(std::numeric_limits<int32_t>::min());
	ws.Write(std::numeric_limits<int32_t>::max());
	ws.Write(std::numeric_limits<uint32_t>::max());
	ws.Write(std::numeric_limits<int64_t>::min());
	ws.Write(std::numeric_limits<int64_t>::max());
	ws.Write(std::numeric_limits<uint64_t>::max());
	ws.Write(std::numeric_limits<float>::max());
	ws.Write(std::numeric_limits<double>::lowest());
	ws.Write(0.0);
	ws.Write(-0.0f);

	CReadMemoryStream rs;
	AttachWritten(rs, ws);

	REQUIRE(rs.Readint16() == std::numeric_limits<int16_t>::min());
	REQUIRE(rs.Readint16() == std::numeric_limits<int16_t>::max());
	REQUIRE(rs.Readintu16() == std::numeric_limits<uint16_t>::max());
	REQUIRE(rs.ReadInt32() == std::numeric_limits<int32_t>::min());
	REQUIRE(rs.ReadInt32() == std::numeric_limits<int32_t>::max());
	REQUIRE(rs.ReadIntu32() == std::numeric_limits<uint32_t>::max());
	REQUIRE(rs.ReadInt64() == std::numeric_limits<int64_t>::min());
	REQUIRE(rs.ReadInt64() == std::numeric_limits<int64_t>::max());
	REQUIRE(rs.ReadIntu64() == std::numeric_limits<uint64_t>::max());
	REQUIRE(rs.ReadFloat() == std::numeric_limits<float>::max());
	REQUIRE(rs.ReadDouble() == std::numeric_limits<double>::lowest());
	REQUIRE(rs.ReadDouble() == 0.0);
	REQUIRE(rs.ReadFloat() == 0.0f);
}

TEST_CASE("Memory stream WriteT/ReadT/ReadTR template round trip", "[stream][memstream][roundtrip]")
{
	test_utils::SPod pod;
	pod.a = -42;
	pod.b = 777;
	pod.c = 1.0 / 3.0;
	for (size_t i = 0; i < sizeof(pod.d); ++i)
		pod.d[i] = byte_t(0xF0 + i);

	CWriteMemoryStream ws;
	ws.WriteT(pod);
	ws.WriteT(uint32_t(0x11223344));
	REQUIRE(ws.Pos() == sizeof(pod) + sizeof(uint32_t));

	CReadMemoryStream rs;
	AttachWritten(rs, ws);

	test_utils::SPod back = {};
	rs.ReadT(back);
	REQUIRE(back == pod);
	REQUIRE(rs.ReadTR<uint32_t>() == 0x11223344);
	REQUIRE(rs.Pos() == rs.Size());
}

TEST_CASE("Memory stream std::string round trip", "[stream][memstream][roundtrip][string]")
{
	CWriteMemoryStream ws;
	const std::string s1 = "hello, stream";
	const std::string s2(300, 'x');   // longer than the growth step
	const std::string s3;             // empty
	const std::string s4("emb\0edded", 9); // embedded NUL is preserved by length-prefixing

	ws.Write(s1);
	ws.Write(s2);
	ws.Write(s3);
	ws.Write(s4);

	REQUIRE(ws.Pos() == 4 * sizeof(uint32_t) + s1.size() + s2.size() + s3.size() + s4.size());

	CReadMemoryStream rs;
	AttachWritten(rs, ws);

	SECTION("length prefix is a uint32_t")
	{
		REQUIRE(rs.ReadIntu32() == (uint32_t)s1.size());
		rs.Reset();
	}

	SECTION("Read(std::string&)")
	{
		std::string r1, r2, r3, r4;
		rs.Read(r1); rs.Read(r2); rs.Read(r3); rs.Read(r4);
		REQUIRE(r1 == s1);
		REQUIRE(r2 == s2);
		REQUIRE(r3 == s3);
		REQUIRE(r4 == s4);
		REQUIRE(rs.Pos() == rs.Size());
	}

	SECTION("ReadAstr()")
	{
		REQUIRE(rs.ReadAstr() == s1);
		REQUIRE(rs.ReadAstr() == s2);
		REQUIRE(rs.ReadAstr() == s3);
		REQUIRE(rs.ReadAstr() == s4);
	}
}

TEST_CASE("Memory stream reading an empty string clears the target", "[stream][memstream][string]")
{
	CWriteMemoryStream ws;
	ws.Write(std::string());
	ws.Write(std::wstring());

	CReadMemoryStream rs;
	AttachWritten(rs, ws);

	std::string target = "stale";
	rs.Read(target);
	REQUIRE(target.empty());

	std::wstring wtarget = L"stale";
	rs.Read(wtarget);
	REQUIRE(wtarget.empty());

	rs.Reset();
	target = "stale";
	REQUIRE(rs.ReadSafe(target));
	REQUIRE(target.empty());
}

TEST_CASE("Memory stream std::wstring round trip", "[stream][memstream][roundtrip][string]")
{
	CWriteMemoryStream ws;
	const std::wstring w1 = L"wide \x0416\x0424 text";
	const std::wstring w2;
	ws.Write(w1);
	ws.Write(w2);

	REQUIRE(ws.Pos() == 2 * sizeof(uint32_t) + w1.size() * sizeof(wchar_t));

	CReadMemoryStream rs;
	AttachWritten(rs, ws);

	std::wstring r1;
	rs.Read(r1);
	REQUIRE(r1 == w1);
	REQUIRE(rs.ReadWstr() == w2);
	REQUIRE(rs.Pos() == rs.Size());
}

TEST_CASE("Memory stream Write(const char*) writes raw bytes without a length prefix", "[stream][memstream][string]")
{
	CWriteMemoryStream ws;
	ws.Write("abc");
	REQUIRE(ws.Pos() == 3);
	REQUIRE(std::memcmp(ws.Buffer(), "abc", 3) == 0);
}

TEST_CASE("Memory stream Write(const wchar_t*) writes raw wchar_t bytes without a length prefix", "[stream][memstream][string]")
{
	CWriteMemoryStream ws;
	const wchar_t* text = L"wide";
	ws.Write(text);
	REQUIRE(ws.Pos() == 4 * sizeof(wchar_t));
	REQUIRE(std::memcmp(ws.Buffer(), text, 4 * sizeof(wchar_t)) == 0);
}

TEST_CASE("Memory stream bool is stored as a single byte 0/1", "[stream][memstream]")
{
	CWriteMemoryStream ws;
	ws.Write(true);
	ws.Write(false);
	REQUIRE(ws.Pos() == 2);
	REQUIRE(ws.Buffer()[0] == 1);
	REQUIRE(ws.Buffer()[1] == 0);

	// only exactly 1 reads back as true
	byte_t raw[3] = { 1, 0, 2 };
	CReadMemoryStream rs;
	rs.AttachBuffer(raw, 3);
	REQUIRE(rs.ReadBool() == true);
	REQUIRE(rs.ReadBool() == false);
	REQUIRE(rs.ReadBool() == false);
}

TEST_CASE("Memory stream little-endian byte layout on a little-endian host", "[stream][memstream]")
{
	if (CommonLib::IStream::IsBigEndian())
		SKIP("byte layout check only meaningful on a little-endian host");

	CWriteMemoryStream ws;
	ws.Write(uint32_t(0x04030201));
	REQUIRE(ws.Pos() == 4);
	REQUIRE(ws.Buffer()[0] == 0x01);
	REQUIRE(ws.Buffer()[1] == 0x02);
	REQUIRE(ws.Buffer()[2] == 0x03);
	REQUIRE(ws.Buffer()[3] == 0x04);
}

// ---------------------------------------------------------------------------
// CReadMemoryStream - bounds
// ---------------------------------------------------------------------------

TEST_CASE("CReadMemoryStream reads exactly what was attached", "[stream][memstream][read]")
{
	std::vector<byte_t> data = MakePattern(64, 9);
	CReadMemoryStream rs;
	rs.AttachBuffer(data.data(), data.size());

	REQUIRE(rs.Size() == 64);
	REQUIRE(rs.Buffer() == data.data());

	std::vector<byte_t> out(64, 0);
	REQUIRE(rs.ReadBytes(out.data(), 10) == 10);
	REQUIRE(rs.Pos() == 10);
	REQUIRE(rs.Read(out.data() + 10, 54) == 54);
	REQUIRE(rs.Pos() == 64);
	REQUIRE(out == data);
}

TEST_CASE("CReadMemoryStream throws on reading past the end and keeps Pos", "[stream][memstream][read]")
{
	std::vector<byte_t> data = MakePattern(8);
	CReadMemoryStream rs;
	rs.AttachBuffer(data.data(), data.size());

	SECTION("partial then overflow")
	{
		REQUIRE(rs.ReadIntu32() == *reinterpret_cast<const uint32_t*>(data.data()));
		REQUIRE(rs.Pos() == 4);
		uint64_t v = 0;
		REQUIRE_THROWS_AS(rs.Read(v), CExcBase);
		REQUIRE(rs.Pos() == 4);

		// the remaining 4 bytes are still readable afterwards
		REQUIRE_NOTHROW(rs.ReadIntu32());
		REQUIRE(rs.Pos() == 8);
	}

	SECTION("exactly at the end")
	{
		REQUIRE_NOTHROW(rs.ReadIntu64());
		REQUIRE(rs.Pos() == 8);
		REQUIRE_THROWS_AS(rs.ReadByte(), CExcBase);
		REQUIRE(rs.Pos() == 8);
	}

	SECTION("a read of 0 bytes at the end is fine")
	{
		rs.Seek(8, soFromBegin);
		byte_t dummy;
		REQUIRE(rs.ReadBytes(&dummy, 0) == 0);
	}
}

TEST_CASE("CReadMemoryStream without a buffer throws on read", "[stream][memstream][read]")
{
	CReadMemoryStream rs;
	REQUIRE(rs.Size() == 0);
	REQUIRE(rs.Buffer() == nullptr);
	REQUIRE_THROWS_AS(rs.ReadByte(), CExcBase);
}

TEST_CASE("CReadMemoryStream ReadSafe returns false past the end", "[stream][memstream][read][safe]")
{
	byte_t data[2] = { 1, 2 };
	CReadMemoryStream rs;
	rs.AttachBuffer(data, 2);

	uint32_t v = 0xFFFFFFFF;
	REQUIRE_FALSE(rs.ReadSafe(v));
	REQUIRE(v == 0xFFFFFFFF); // untouched
	REQUIRE(rs.Pos() == 0);

	std::string s;
	REQUIRE_FALSE(rs.ReadSafe(s)); // needs 4 bytes of length prefix

	// the two bytes that are there can still be read safely
	uint16_t u16 = 0;
	REQUIRE(rs.ReadSafe(u16));
	REQUIRE(u16 == 0x0201);
	REQUIRE_FALSE(rs.ReadSafe(u16));
}

TEST_CASE("CReadMemoryStream ReadSafe delivers the value", "[stream][memstream][read][safe]")
{
	const std::string longText(100, 'q');
	const std::wstring wide = L"wide safe text";

	CWriteMemoryStream ws;
	ws.Write(true);
	ws.Write(int32_t(-77));
	ws.Write(uint64_t(0x0102030405060708ULL));
	ws.Write(1.5);
	ws.Write(std::string("abc"));
	ws.Write(longText);
	ws.Write(wide);
	const byte_t raw[3] = { 9, 8, 7 };
	ws.Write(raw, 3);

	CReadMemoryStream rs;
	AttachWritten(rs, ws);

	// the typed overloads are callable on the concrete class as well as via IReadStream
	IReadStream& r = rs;

	bool b = false;
	REQUIRE(rs.ReadSafe(b));
	REQUIRE(b == true);

	int32_t i32 = 0;
	REQUIRE(rs.ReadSafe(i32));
	REQUIRE(i32 == -77);

	uint64_t u64 = 0;
	REQUIRE(r.ReadSafe(u64));
	REQUIRE(u64 == 0x0102030405060708ULL);

	double d = 0;
	REQUIRE(rs.ReadSafe(d));
	REQUIRE(d == 1.5);

	std::string s;
	REQUIRE(rs.ReadSafe(s));
	REQUIRE(s == "abc");

	std::string l;
	REQUIRE(r.ReadSafe(l));
	REQUIRE(l == longText);

	std::wstring w;
	REQUIRE(rs.ReadSafe(w));
	REQUIRE(w == wide);

	byte_t rawBack[3] = { 0, 0, 0 };
	REQUIRE(rs.ReadSafe(rawBack, 3) == 3);
	REQUIRE(std::memcmp(raw, rawBack, 3) == 0);
	REQUIRE(rs.Pos() == rs.Size());
}

// ---------------------------------------------------------------------------
// Seek / Reset / Resize
// ---------------------------------------------------------------------------

TEST_CASE("CWriteMemoryStream Seek variants", "[stream][memstream][seek]")
{
	CWriteMemoryStream ws;
	const std::vector<byte_t> data = MakePattern(50);
	ws.Write(data.data(), data.size());
	const size_t cap = ws.Size();
	REQUIRE(cap >= 50);

	SECTION("soFromBegin")
	{
		ws.Seek(10, soFromBegin);
		REQUIRE(ws.Pos() == 10);
	}

	SECTION("soFromCurrent moves forward relative to Pos")
	{
		ws.Seek(10, soFromBegin);
		ws.Seek(5, soFromCurrent);
		REQUIRE(ws.Pos() == 15);
	}

	SECTION("soFromEnd(0) lands on Size (the capacity)")
	{
		ws.Seek(0, soFromEnd);
		REQUIRE(ws.Pos() == cap);
		ws.Seek(3, soFromEnd);
		REQUIRE(ws.Pos() == cap - 3);
	}

	SECTION("SeekSafe returns true for a valid position")
	{
		REQUIRE(ws.SeekSafe(7, soFromBegin));
		REQUIRE(ws.Pos() == 7);
	}

	SECTION("Reset goes back to 0 without touching the data")
	{
		ws.Reset();
		REQUIRE(ws.Pos() == 0);
		REQUIRE(SameBytes(ws.Buffer(), data));
	}

	SECTION("seeking back and writing overwrites in place")
	{
		ws.Seek(10, soFromBegin);
		ws.Write(uint32_t(0xAAAAAAAA));
		REQUIRE(ws.Pos() == 14);

		std::vector<byte_t> expected = data;
		expected[10] = expected[11] = expected[12] = expected[13] = 0xAA;
		REQUIRE(SameBytes(ws.Buffer(), expected));
	}

	SECTION("seeking past the end grows the buffer to exactly that position")
	{
		ws.Seek(cap + 100, soFromBegin);
		REQUIRE(ws.Pos() == cap + 100);
		REQUIRE(ws.Size() == cap + 100);
		REQUIRE(SameBytes(ws.Buffer(), data)); // prefix preserved
	}
}

TEST_CASE("Seek on an empty memory stream fails", "[stream][memstream][seek]")
{
	CWriteMemoryStream ws;
	REQUIRE_THROWS_AS(ws.Seek(0, soFromBegin), CExcBase);
	REQUIRE_FALSE(ws.SeekSafe(0, soFromBegin));

	CReadMemoryStream rs;
	REQUIRE_THROWS_AS(rs.Seek(0, soFromBegin), CExcBase);
	REQUIRE_FALSE(rs.SeekSafe(0, soFromBegin));
}

TEST_CASE("CReadMemoryStream Seek stays within the attached buffer", "[stream][memstream][seek]")
{
	std::vector<byte_t> data = MakePattern(16);
	CReadMemoryStream rs;
	rs.AttachBuffer(data.data(), data.size());

	rs.Seek(12, soFromBegin);
	REQUIRE(rs.Pos() == 12);
	REQUIRE(rs.ReadIntu32() == *reinterpret_cast<const uint32_t*>(data.data() + 12));

	rs.Seek(4, soFromEnd);
	REQUIRE(rs.Pos() == 12);

	rs.Seek(16, soFromBegin); // exactly at the end is allowed
	REQUIRE(rs.Pos() == 16);

	// past the end: a read stream cannot grow
	REQUIRE_THROWS_AS(rs.Seek(17, soFromBegin), CExcBase);
	REQUIRE_FALSE(rs.SeekSafe(1, soFromCurrent));
	REQUIRE(rs.Pos() == 16);
	REQUIRE(rs.Size() == 16);
}

TEST_CASE("CWriteMemoryStream Resize", "[stream][memstream][resize]")
{
	CWriteMemoryStream ws;
	const std::vector<byte_t> data = MakePattern(20);
	ws.Write(data.data(), data.size());
	const size_t cap = ws.Size();

	SECTION("growing preserves data and Pos")
	{
		ws.Resize(cap + 500);
		REQUIRE(ws.Size() == cap + 500);
		REQUIRE(ws.Pos() == 20);
		REQUIRE(SameBytes(ws.Buffer(), data));

		// writing continues where it left off
		ws.Write(uint8_t(0x5A));
		REQUIRE(ws.Buffer()[20] == 0x5A);
	}

	SECTION("shrinking is a no-op")
	{
		ws.Resize(5);
		REQUIRE(ws.Size() == cap);
		REQUIRE(ws.Pos() == 20);
		REQUIRE(SameBytes(ws.Buffer(), data));
	}
}

TEST_CASE("CWriteMemoryStream Close releases the buffer", "[stream][memstream]")
{
	CWriteMemoryStream ws;
	ws.Write(uint32_t(1));
	ws.Close();
	REQUIRE(ws.Size() == 0);
	REQUIRE(ws.Buffer() == nullptr);
}

// ---------------------------------------------------------------------------
// Attached / detached buffers
// ---------------------------------------------------------------------------

TEST_CASE("CWriteMemoryStream writes into an attached external buffer", "[stream][memstream][attach]")
{
	std::vector<byte_t> external(16, 0);
	CWriteMemoryStream ws;
	ws.AttachBuffer(external.data(), external.size());

	REQUIRE(ws.Size() == 16);
	REQUIRE(ws.Buffer() == external.data());

	ws.Write(uint64_t(0x1111111111111111ULL));
	ws.Write(uint64_t(0x2222222222222222ULL));
	REQUIRE(ws.Pos() == 16);
	REQUIRE(external[0] == 0x11);
	REQUIRE(external[15] == 0x22);

	SECTION("an attached buffer cannot grow")
	{
		REQUIRE_THROWS_AS(ws.Write(uint8_t(1)), CExcBase);
		REQUIRE_THROWS_AS(ws.Resize(32), CExcBase);
		REQUIRE_THROWS_AS(ws.Seek(17, soFromBegin), CExcBase);
		REQUIRE(ws.Size() == 16);
		REQUIRE(ws.Buffer() == external.data());
	}

	SECTION("WriteSafe reports the failure instead of throwing")
	{
		REQUIRE_FALSE(ws.WriteSafe(uint8_t(1)));
		REQUIRE(ws.Pos() == 16);
	}
}

TEST_CASE("CWriteMemoryStream AttachBuffer with copy leaves the original intact and can grow", "[stream][memstream][attach]")
{
	std::vector<byte_t> external = MakePattern(8, 5);
	const std::vector<byte_t> original = external;

	CWriteMemoryStream ws;
	ws.AttachBuffer(external.data(), external.size(), true);

	REQUIRE(ws.Buffer() != external.data());
	REQUIRE(SameBytes(ws.Buffer(), original));

	ws.Seek(0, soFromEnd);
	ws.Write(uint32_t(0xCAFEBABE)); // grows past the copied 8 bytes
	REQUIRE(ws.Pos() == 12);
	REQUIRE(ws.Size() >= 12);
	REQUIRE(SameBytes(ws.Buffer(), original));
	REQUIRE(external == original);
}

TEST_CASE("CWriteMemoryStream DeattachBuffer transfers ownership", "[stream][memstream][attach][alloc]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	byte_t* detached = nullptr;
	const std::vector<byte_t> data = MakePattern(24);

	{
		CWriteMemoryStream ws(alloc);
		ws.Write(data.data(), data.size());
		byte_t* internal = ws.Buffer();

		detached = ws.DeattachBuffer();

		REQUIRE(detached == internal);
		REQUIRE(ws.Pos() == 0);
		REQUIRE(ws.Size() == 0);
		REQUIRE(ws.Buffer() == nullptr);

		// the stream is usable again afterwards
		ws.Write(uint8_t(1));
		REQUIRE(ws.Pos() == 1);
	}

	REQUIRE(SameBytes(detached, data));
	REQUIRE(alloc->GetCurrentMemoryBalance() > 0);
	alloc->Free(detached);
	REQUIRE(alloc->GetCurrentMemoryBalance() == 0);
}

TEST_CASE("CWriteMemoryStream AttachBuffer(IMemStreamBufferPtr) shares the buffer object", "[stream][memstream][attach]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	IMemStreamBufferPtr shared = std::make_shared<CMemoryStreamBuffer>(alloc);
	shared->Create(8);

	CWriteMemoryStream ws(alloc);
	ws.AttachBuffer(shared);
	REQUIRE(ws.Buffer() == shared->GetData());
	REQUIRE(ws.Size() == 8);

	ws.Write(uint32_t(0x01020304));
	REQUIRE(shared->GetData()[0] == ws.Buffer()[0]);
}

TEST_CASE("CWriteMemoryStream returns all memory to a custom allocator", "[stream][memstream][alloc]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);

	{
		CWriteMemoryStream ws(alloc);
		for (int i = 0; i < 2000; ++i)
			ws.Write(int32_t(i));

		REQUIRE(alloc->GetAllocCount() > 1); // it had to grow several times
		REQUIRE(alloc->GetCurrentMemoryBalance() == (int64_t)ws.Size());

		CReadMemoryStream rs;
		AttachWritten(rs, ws);
		for (int i = 0; i < 2000; ++i)
			REQUIRE(rs.ReadInt32() == i);
	}

	REQUIRE(alloc->GetCurrentMemoryBalance() == 0);
	REQUIRE(alloc->GetAllocCount() == alloc->GetFreeCount());
}

// ---------------------------------------------------------------------------
// Safe writers
// ---------------------------------------------------------------------------

TEST_CASE("CWriteMemoryStream WriteSafe scalars succeed and are readable", "[stream][memstream][write][safe]")
{
	CWriteMemoryStream ws;
	REQUIRE(ws.WriteSafe(true));
	REQUIRE(ws.WriteSafe(uint8_t(7)));
	REQUIRE(ws.WriteSafe(char('q')));
	REQUIRE(ws.WriteSafe(int16_t(-3)));
	REQUIRE(ws.WriteSafe(uint16_t(3)));
	REQUIRE(ws.WriteSafe(int32_t(-5)));
	REQUIRE(ws.WriteSafe(uint32_t(5)));
	REQUIRE(ws.WriteSafe(int64_t(-9)));
	REQUIRE(ws.WriteSafe(uint64_t(9)));
	REQUIRE(ws.WriteSafe(2.5f));
	REQUIRE(ws.WriteSafe(-8.75));

	const byte_t raw[3] = { 1, 2, 3 };
	REQUIRE(ws.WriteSafe(raw, 3));
	REQUIRE(ws.WriteTSafe(uint16_t(0xBEEF)));

	CReadMemoryStream rs;
	AttachWritten(rs, ws);
	REQUIRE(rs.ReadBool() == true);
	REQUIRE(rs.ReadByte() == 7);
	REQUIRE(rs.ReadChar() == 'q');
	REQUIRE(rs.Readint16() == -3);
	REQUIRE(rs.Readintu16() == 3);
	REQUIRE(rs.ReadInt32() == -5);
	REQUIRE(rs.ReadIntu32() == 5);
	REQUIRE(rs.ReadInt64() == -9);
	REQUIRE(rs.ReadIntu64() == 9);
	REQUIRE(rs.ReadFloat() == 2.5f);
	REQUIRE(rs.ReadDouble() == -8.75);
	byte_t rawBack[3] = { 0, 0, 0 };
	rs.Read(rawBack, 3);
	REQUIRE(std::memcmp(raw, rawBack, 3) == 0);
	REQUIRE(rs.Readintu16() == 0xBEEF);
	REQUIRE(rs.Pos() == rs.Size());
}

TEST_CASE("CWriteMemoryStream WriteSafe strings succeed and are readable", "[stream][memstream][write][safe][string]")
{
	CWriteMemoryStream ws;
	const std::string s = "safe string";
	const std::wstring w = L"safe wide";

	REQUIRE(ws.WriteSafe(s));
	REQUIRE(ws.WriteSafe(w));
	REQUIRE(ws.WriteSafe(std::string()));  // empty strings are just a zero length prefix
	REQUIRE(ws.WriteSafe(std::wstring()));
	REQUIRE(ws.Pos() == 4 * sizeof(uint32_t) + s.size() + w.size() * sizeof(wchar_t));

	CReadMemoryStream rs;
	AttachWritten(rs, ws);
	REQUIRE(rs.ReadAstr() == s);
	REQUIRE(rs.ReadWstr() == w);
	REQUIRE(rs.ReadAstr().empty());
	REQUIRE(rs.ReadWstr().empty());
	REQUIRE(rs.Pos() == rs.Size());
}

TEST_CASE("CFxMemoryWriteStream WriteSafe string fails cleanly when it does not fit", "[stream][memstream][write][safe][string]")
{
	std::vector<byte_t> external(6, 0);
	CFxMemoryWriteStream ws;
	ws.AttachBuffer(external.data(), external.size());

	// length prefix (4) fits, the 5 characters do not
	REQUIRE_FALSE(ws.WriteSafe(std::string("hello")));
	REQUIRE(ws.Pos() == 4);
}

// ---------------------------------------------------------------------------
// Inverse (byte-swapped) paths. These are only used automatically on a
// big-endian host, but they are public and should be correct on their own.
// ---------------------------------------------------------------------------

TEST_CASE("CWriteMemoryStream WriteInverse reverses the byte order", "[stream][memstream][inverse]")
{
	CWriteMemoryStream ws;
	const byte_t in[4] = { 1, 2, 3, 4 };

	REQUIRE(ws.WriteInverse(in, 4) == 4);
	REQUIRE(ws.Pos() == 4);

	const byte_t expected[4] = { 4, 3, 2, 1 };
	REQUIRE(std::memcmp(ws.Buffer(), expected, 4) == 0);

	// a second call appends after the first
	REQUIRE(ws.WriteInverse(in, 2) == 2);
	REQUIRE(ws.Pos() == 6);
	REQUIRE(ws.Buffer()[4] == 2);
	REQUIRE(ws.Buffer()[5] == 1);

	// grows like WriteBytes does
	const std::vector<byte_t> big = MakePattern(500);
	REQUIRE(ws.WriteInverse(big.data(), big.size()) == 500);
	REQUIRE(ws.Pos() == 506);
	for (size_t i = 0; i < big.size(); ++i)
		REQUIRE(ws.Buffer()[6 + i] == big[big.size() - i - 1]);
}

TEST_CASE("CReadMemoryStream ReadInverse reverses the byte order", "[stream][memstream][inverse]")
{
	byte_t data[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	CReadMemoryStream rs;
	rs.AttachBuffer(data, 8);

	byte_t out[4] = { 0, 0, 0, 0 };
	REQUIRE(rs.ReadInverse(out, 4) == 4);
	REQUIRE(rs.Pos() == 4);
	const byte_t expected[4] = { 4, 3, 2, 1 };
	REQUIRE(std::memcmp(out, expected, 4) == 0);

	REQUIRE(rs.ReadInverse(out, 4) == 4);
	REQUIRE(rs.Pos() == 8);
	const byte_t expected2[4] = { 8, 7, 6, 5 };
	REQUIRE(std::memcmp(out, expected2, 4) == 0);
}

TEST_CASE("WriteInverse followed by ReadInverse restores the original values", "[stream][memstream][inverse][roundtrip]")
{
	CWriteMemoryStream ws;
	const uint32_t a = 0x11223344;
	const uint64_t b = 0x0102030405060708ULL;
	const double c = 123.456;
	ws.WriteInverse((const byte_t*)&a, sizeof(a));
	ws.WriteInverse((const byte_t*)&b, sizeof(b));
	ws.WriteInverse((const byte_t*)&c, sizeof(c));

	CReadMemoryStream rs;
	AttachWritten(rs, ws);
	uint32_t ra = 0; uint64_t rb = 0; double rc = 0;
	rs.ReadInverse((byte_t*)&ra, sizeof(ra));
	rs.ReadInverse((byte_t*)&rb, sizeof(rb));
	rs.ReadInverse((byte_t*)&rc, sizeof(rc));
	REQUIRE(ra == a);
	REQUIRE(rb == b);
	REQUIRE(rc == c);
	REQUIRE(rs.Pos() == rs.Size());
}

TEST_CASE("CReadMemoryStream ReadInverse throws past the end", "[stream][memstream][inverse]")
{
	byte_t data[2] = { 1, 2 };
	CReadMemoryStream rs;
	rs.AttachBuffer(data, 2);
	byte_t out[4];
	REQUIRE_THROWS_AS(rs.ReadInverse(out, 4), CExcBase);
}
