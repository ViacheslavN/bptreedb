#include "TestCommon.h"

using namespace CommonLib;
using test_utils::MakePattern;
using test_utils::SameBytes;
using test_utils::AttachWritten;

TEST_CASE("CFxMemoryWriteStream initial state", "[stream][fxmemstream]")
{
	CFxMemoryWriteStream ws;
	REQUIRE(ws.Pos() == 0);
	REQUIRE(ws.Size() == 0);
	REQUIRE(ws.Buffer() == nullptr);
	REQUIRE(ws.IsEnoughSpace(0));
	REQUIRE_FALSE(ws.IsEnoughSpace(1));
}

TEST_CASE("CFxMemoryWriteStream has no allocator and cannot Create", "[stream][fxmemstream]")
{
	CFxMemoryWriteStream ws;
	REQUIRE_THROWS_AS(ws.Create(16), CExcBase);
	REQUIRE(ws.Size() == 0);
}

TEST_CASE("CFxMemoryWriteStream writes into the attached buffer and never grows", "[stream][fxmemstream]")
{
	std::vector<byte_t> external(16, 0);
	CFxMemoryWriteStream ws;
	ws.AttachBuffer(external.data(), external.size());

	REQUIRE(ws.Size() == 16);
	REQUIRE(ws.Buffer() == external.data());
	REQUIRE(ws.IsEnoughSpace(16));
	REQUIRE_FALSE(ws.IsEnoughSpace(17));

	const std::vector<byte_t> data = MakePattern(16, 4);
	REQUIRE(ws.WriteBytes(data.data(), 10) == 10);
	REQUIRE(ws.Pos() == 10);
	REQUIRE(ws.IsEnoughSpace(6));
	REQUIRE_FALSE(ws.IsEnoughSpace(7));

	REQUIRE(ws.Write(data.data() + 10, 6) == 6);
	REQUIRE(ws.Pos() == 16);
	REQUIRE(external == data);

	SECTION("WriteBytes past the end throws and leaves Pos and data untouched")
	{
		byte_t extra = 0xFF;
		REQUIRE_THROWS_AS(ws.WriteBytes(&extra, 1), CExcBase);
		REQUIRE_THROWS_AS(ws.Write(uint32_t(1)), CExcBase);
		REQUIRE(ws.Pos() == 16);
		REQUIRE(external == data);
	}

	SECTION("WriteSafe past the end returns false instead of throwing")
	{
		REQUIRE_FALSE(ws.WriteSafe(uint8_t(1)));
		REQUIRE_FALSE(ws.WriteSafe(uint64_t(1)));
		byte_t extra[2] = { 1, 2 };
		REQUIRE_FALSE(ws.WriteSafe(extra, 2));
		REQUIRE(ws.Pos() == 16);
		REQUIRE(external == data);
	}

	SECTION("a zero-length write at the end is fine")
	{
		byte_t dummy = 0;
		REQUIRE(ws.WriteBytes(&dummy, 0) == 0);
		REQUIRE(ws.Pos() == 16);
	}

	SECTION("Resize is not supported")
	{
		REQUIRE_THROWS_AS(ws.Resize(32), CExcBase);
		REQUIRE(ws.Size() == 16);
	}
}

TEST_CASE("CFxMemoryWriteStream WriteSafe succeeds while there is room", "[stream][fxmemstream][safe]")
{
	std::vector<byte_t> external(8, 0);
	CFxMemoryWriteStream ws;
	ws.AttachBuffer(external.data(), external.size());

	REQUIRE(ws.WriteSafe(uint32_t(0x01020304)));
	REQUIRE(ws.Pos() == 4);
	REQUIRE(ws.WriteSafe(uint16_t(0x0506)));
	REQUIRE(ws.Pos() == 6);
	REQUIRE_FALSE(ws.WriteSafe(uint32_t(0))); // 4 bytes do not fit into the remaining 2
	REQUIRE(ws.Pos() == 6);
	REQUIRE(ws.WriteSafe(uint16_t(0x0708)));
	REQUIRE(ws.Pos() == 8);
	REQUIRE_FALSE(ws.WriteSafe(uint8_t(0)));

	CReadMemoryStream rs;
	rs.AttachBuffer(external.data(), external.size());
	REQUIRE(rs.ReadIntu32() == 0x01020304);
	REQUIRE(rs.Readintu16() == 0x0506);
	REQUIRE(rs.Readintu16() == 0x0708);
}

TEST_CASE("CFxMemoryWriteStream scalar and string round trip", "[stream][fxmemstream][roundtrip]")
{
	std::vector<byte_t> external(256, 0);
	CFxMemoryWriteStream ws;
	ws.AttachBuffer(external.data(), external.size());

	const std::string text = "fixed size stream";
	ws.Write(true);
	ws.Write(int16_t(-321));
	ws.Write(uint32_t(0xABCDEF01));
	ws.Write(int64_t(-99));
	ws.Write(1.25f);
	ws.Write(9.75);
	ws.Write(text);

	const size_t written = 1 + 2 + 4 + 8 + 4 + 8 + 4 + text.size();
	REQUIRE(ws.Pos() == written);

	CReadMemoryStream rs;
	AttachWritten(rs, ws);
	REQUIRE(rs.Size() == written);

	REQUIRE(rs.ReadBool() == true);
	REQUIRE(rs.Readint16() == -321);
	REQUIRE(rs.ReadIntu32() == 0xABCDEF01);
	REQUIRE(rs.ReadInt64() == -99);
	REQUIRE(rs.ReadFloat() == 1.25f);
	REQUIRE(rs.ReadDouble() == 9.75);
	REQUIRE(rs.ReadAstr() == text);
	REQUIRE(rs.Pos() == rs.Size());
}

TEST_CASE("CFxMemoryWriteStream Seek", "[stream][fxmemstream][seek]")
{
	std::vector<byte_t> external(32, 0);
	CFxMemoryWriteStream ws;
	ws.AttachBuffer(external.data(), external.size());

	SECTION("within the buffer")
	{
		ws.Seek(8, soFromBegin);
		REQUIRE(ws.Pos() == 8);
		ws.Write(uint32_t(0xAAAAAAAA));
		REQUIRE(external[8] == 0xAA);
		REQUIRE(external[11] == 0xAA);
		REQUIRE(external[7] == 0);
		REQUIRE(external[12] == 0);

		ws.Seek(4, soFromCurrent);
		REQUIRE(ws.Pos() == 16);
		ws.Seek(0, soFromEnd);
		REQUIRE(ws.Pos() == 32);
		REQUIRE(ws.SeekSafe(1, soFromBegin));
		ws.Reset();
		REQUIRE(ws.Pos() == 0);
	}

	SECTION("past the end fails because a fixed stream cannot resize")
	{
		REQUIRE_THROWS_AS(ws.Seek(33, soFromBegin), CExcBase);
		REQUIRE_FALSE(ws.SeekSafe(33, soFromBegin));
		REQUIRE(ws.Pos() == 0);
		REQUIRE(ws.Size() == 32);
	}
}

TEST_CASE("CFxMemoryWriteStream DeattachBuffer", "[stream][fxmemstream]")
{
	std::vector<byte_t> external(8, 0);
	CFxMemoryWriteStream ws;
	ws.AttachBuffer(external.data(), external.size());
	ws.Write(uint32_t(1));

	REQUIRE(ws.DeattachBuffer() == external.data());
	REQUIRE(ws.Pos() == 0);
	REQUIRE(ws.Size() == 0);
	REQUIRE(ws.Buffer() == nullptr);
	REQUIRE_FALSE(ws.IsEnoughSpace(1));
}

TEST_CASE("CFxMemoryWriteStream WriteInverse reverses the byte order", "[stream][fxmemstream][inverse]")
{
	std::vector<byte_t> external(8, 0);
	CFxMemoryWriteStream ws;
	ws.AttachBuffer(external.data(), external.size());

	const byte_t in[4] = { 1, 2, 3, 4 };
	REQUIRE(ws.WriteInverse(in, 4) == 4);
	REQUIRE(ws.Pos() == 4);
	const byte_t expected[4] = { 4, 3, 2, 1 };
	REQUIRE(std::memcmp(external.data(), expected, 4) == 0);

	REQUIRE(ws.WriteInverse(in, 4) == 4);
	REQUIRE(ws.Pos() == 8);
	REQUIRE(std::memcmp(external.data() + 4, expected, 4) == 0);
	REQUIRE_THROWS_AS(ws.WriteInverse(in, 1), CExcBase);
}

TEST_CASE("CFxMemoryWriteStream WriteInverse past the end throws", "[stream][fxmemstream][inverse]")
{
	std::vector<byte_t> external(2, 0);
	CFxMemoryWriteStream ws;
	ws.AttachBuffer(external.data(), external.size());
	const byte_t in[4] = { 1, 2, 3, 4 };
	REQUIRE_THROWS_AS(ws.WriteInverse(in, 4), CExcBase);
	REQUIRE(ws.Pos() == 0);
}
