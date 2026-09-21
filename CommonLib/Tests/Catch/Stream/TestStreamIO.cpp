#include "TestCommon.h"
#include "CommonLib/stream/StreamIO.h"
#include "CommonLib/stream/StdLineInOut.h"

#include <sstream>

using namespace CommonLib;
using test_utils::MakePattern;

namespace
{
	// Minimal in-memory io::IWrite / io::IRead pair sharing one byte vector,
	// standing in for a socket, pipe or file behind CWriteStreamIO/CReadStreamIO.
	struct SByteSink
	{
		std::vector<byte_t> bytes;
		size_t readPos = 0;
	};

	class CVectorWriter : public io::IWrite
	{
	public:
		explicit CVectorWriter(std::shared_ptr<SByteSink> sink) : m_sink(std::move(sink)) {}

		std::streamsize Write(const byte_t* dataPtr, size_t dataSize) override
		{
			m_sink->bytes.insert(m_sink->bytes.end(), dataPtr, dataPtr + dataSize);
			++calls;
			return (std::streamsize)dataSize;
		}

		int calls = 0;
	private:
		std::shared_ptr<SByteSink> m_sink;
	};

	class CVectorReader : public io::IRead
	{
	public:
		explicit CVectorReader(std::shared_ptr<SByteSink> sink) : m_sink(std::move(sink)) {}

		std::streamsize Read(byte_t* dataPtr, size_t dataSize) override
		{
			const size_t avail = m_sink->bytes.size() - m_sink->readPos;
			const size_t n = std::min(avail, dataSize);
			std::memcpy(dataPtr, m_sink->bytes.data() + m_sink->readPos, n);
			m_sink->readPos += n;
			return (std::streamsize)n;
		}
	private:
		std::shared_ptr<SByteSink> m_sink;
	};
}

// ---------------------------------------------------------------------------
// CWriteStreamIO / CReadStreamIO
// ---------------------------------------------------------------------------

TEST_CASE("CWriteStreamIO forwards raw bytes to the io::IWrite device", "[stream][streamio]")
{
	auto sink = std::make_shared<SByteSink>();
	auto device = std::make_shared<CVectorWriter>(sink);
	CWriteStreamIO ws(device);

	const std::vector<byte_t> data = MakePattern(37);
	REQUIRE(ws.WriteBytes(data.data(), data.size()) == (std::streamsize)data.size());
	REQUIRE(sink->bytes == data);
	REQUIRE(device->calls == 1);

	ws.Write(uint32_t(0x01020304));
	REQUIRE(sink->bytes.size() == 41);
	REQUIRE(device->calls == 2);
	if (!CommonLib::IStream::IsBigEndian())
	{
		REQUIRE(sink->bytes[37] == 0x04);
		REQUIRE(sink->bytes[40] == 0x01);
	}
}

TEST_CASE("CReadStreamIO pulls bytes from the io::IRead device", "[stream][streamio]")
{
	auto sink = std::make_shared<SByteSink>();
	sink->bytes = MakePattern(20, 8);
	CReadStreamIO rs(std::make_shared<CVectorReader>(sink));

	std::vector<byte_t> out(20, 0);
	REQUIRE(rs.ReadBytes(out.data(), 5) == 5);
	REQUIRE(rs.ReadBytes(out.data() + 5, 15) == 15);
	REQUIRE(out == sink->bytes);

	SECTION("a short read reports the number of bytes actually delivered")
	{
		byte_t more[4];
		REQUIRE(rs.ReadBytes(more, 4) == 0);
	}
}

TEST_CASE("CWriteStreamIO -> CReadStreamIO round trip of scalars and strings", "[stream][streamio][roundtrip]")
{
	auto sink = std::make_shared<SByteSink>();
	CWriteStreamIO ws(std::make_shared<CVectorWriter>(sink));

	const std::string text = "through an io device";
	const std::wstring wtext = L"wide io";

	ws.Write(false);
	ws.Write(uint8_t(200));
	ws.Write(int16_t(-2));
	ws.Write(uint32_t(123456));
	ws.Write(int64_t(-987654321LL));
	ws.Write(0.5f);
	ws.Write(100.125);
	ws.Write(text);
	ws.Write(wtext);
	ws.Write(std::string());

	const size_t expected = 1 + 1 + 2 + 4 + 8 + 4 + 8
		+ 4 + text.size()
		+ 4 + wtext.size() * sizeof(wchar_t)
		+ 4;
	REQUIRE(sink->bytes.size() == expected);

	CReadStreamIO rs(std::make_shared<CVectorReader>(sink));
	REQUIRE(rs.ReadBool() == false);
	REQUIRE(rs.ReadByte() == 200);
	REQUIRE(rs.Readint16() == -2);
	REQUIRE(rs.ReadIntu32() == 123456);
	REQUIRE(rs.ReadInt64() == -987654321LL);
	REQUIRE(rs.ReadFloat() == 0.5f);
	REQUIRE(rs.ReadDouble() == 100.125);
	REQUIRE(rs.ReadAstr() == text);
	REQUIRE(rs.ReadWstr() == wtext);
	REQUIRE(rs.ReadAstr().empty());
	REQUIRE(sink->readPos == sink->bytes.size());
}

TEST_CASE("CWriteStreamIO and CReadStreamIO count transferred bytes in Pos/Size", "[stream][streamio]")
{
	auto sink = std::make_shared<SByteSink>();
	CWriteStreamIO ws(std::make_shared<CVectorWriter>(sink));
	REQUIRE(ws.Pos() == 0);
	REQUIRE(ws.Size() == 0);

	ws.Write(uint64_t(1));
	ws.Write(std::string("abc"));
	REQUIRE(ws.Pos() == 8 + 4 + 3);
	REQUIRE(ws.Size() == ws.Pos());

	CReadStreamIO rs(std::make_shared<CVectorReader>(sink));
	REQUIRE(rs.Pos() == 0);
	rs.ReadIntu64();
	REQUIRE(rs.Pos() == 8);
	rs.ReadAstr();
	REQUIRE(rs.Pos() == 15);
	REQUIRE(rs.Size() == 15);

	byte_t more[4];
	REQUIRE(rs.ReadBytes(more, 4) == 0); // device exhausted
	REQUIRE(rs.Pos() == 15);
}

TEST_CASE("CWriteStreamIO WriteInverse and CReadStreamIO ReadInverse swap byte order", "[stream][streamio][inverse]")
{
	auto sink = std::make_shared<SByteSink>();
	CWriteStreamIO ws(std::make_shared<CVectorWriter>(sink));

	const byte_t in[4] = { 1, 2, 3, 4 };
	REQUIRE(ws.WriteInverse(in, 4) == 4);
	const std::vector<byte_t> expected = { 4, 3, 2, 1 };
	REQUIRE(sink->bytes == expected);
	REQUIRE(ws.Pos() == 4);

	CReadStreamIO rs(std::make_shared<CVectorReader>(sink));
	byte_t out[4] = { 0, 0, 0, 0 };
	REQUIRE(rs.ReadInverse(out, 4) == 4);
	REQUIRE(std::memcmp(out, in, 4) == 0);
	REQUIRE(rs.Pos() == 4);
}

TEST_CASE("CWriteStreamIO WriteSafe scalars succeed", "[stream][streamio][safe]")
{
	auto sink = std::make_shared<SByteSink>();
	CWriteStreamIO ws(std::make_shared<CVectorWriter>(sink));

	REQUIRE(ws.WriteSafe(uint16_t(1)));
	REQUIRE(ws.WriteSafe(int32_t(-1)));
	REQUIRE(ws.WriteSafe(3.0));
	REQUIRE(sink->bytes.size() == 2 + 4 + 8);
}

TEST_CASE("CWriteStreamIO propagates device exceptions and WriteSafe swallows them", "[stream][streamio][safe]")
{
	class CFailingWriter : public io::IWrite
	{
	public:
		std::streamsize Write(const byte_t*, size_t) override
		{
			throw CExcBase("device is broken");
		}
	};

	CWriteStreamIO ws(std::make_shared<CFailingWriter>());
	REQUIRE_THROWS_AS(ws.Write(uint32_t(1)), CExcBase);
	REQUIRE_FALSE(ws.WriteSafe(uint32_t(1)));
}

// ---------------------------------------------------------------------------
// io::StdLineInOut
// ---------------------------------------------------------------------------

TEST_CASE("StdLineInOut reads lines until the input is exhausted", "[stream][lineio]")
{
	std::istringstream in("first line\nsecond\n\nlast");
	std::ostringstream out;
	io::StdLineInOut lio(in, out);

	REQUIRE_FALSE(lio.IsEof());
	REQUIRE(lio.GetLine() == "first line");
	REQUIRE(lio.GetLine() == "second");
	REQUIRE(lio.GetLine() == "");   // empty line is a valid line
	REQUIRE_FALSE(lio.IsEof());
	REQUIRE(lio.GetLine() == "last"); // no trailing newline: reading it hits EOF
	REQUIRE(lio.IsEof());
	REQUIRE_THROWS_AS(lio.GetLine(), CExcBase);
}

TEST_CASE("StdLineInOut GetLine throws once nothing is left", "[stream][lineio]")
{
	std::istringstream in("only\n");
	std::ostringstream out;
	io::StdLineInOut lio(in, out);

	REQUIRE(lio.GetLine() == "only");
	REQUIRE_THROWS_AS(lio.GetLine(), CExcBase);
	REQUIRE(lio.IsEof());
}

TEST_CASE("StdLineInOut writes to the output stream", "[stream][lineio]")
{
	std::istringstream in;
	std::ostringstream out;
	io::StdLineInOut lio(in, out);

	lio.Write("abc");
	REQUIRE(out.str() == "abc");
	lio.WriteLine("def");
	REQUIRE(out.str() == "abcdef\n");
	lio.WriteLine("");
	REQUIRE(out.str() == "abcdef\n\n");
}

TEST_CASE("StdLineInOut can be used through the ILineInOut interface", "[stream][lineio]")
{
	std::istringstream in("ping\n");
	std::ostringstream out;
	io::ILineInOutPtr lio = std::make_shared<io::StdLineInOut>(in, out);

	std::string line = lio->GetLine();
	lio->WriteLine("pong: " + line);
	REQUIRE(out.str() == "pong: ping\n");
}
