#include "TestCommon.h"
#include "CommonLib/stream/FileStream.h"
#include "CommonLib/filesystem/File.h"

#include <filesystem>
#include <chrono>
#include <atomic>

using namespace CommonLib;
using test_utils::MakePattern;

namespace
{
	namespace fs = std::filesystem;

	// A unique file in the system temp directory, removed on scope exit.
	struct STempFile
	{
		fs::path path;

		STempFile()
		{
			static std::atomic<unsigned> counter{ 0 };
			const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
			path = fs::temp_directory_path() /
				("bptreedb_stream_test_" + std::to_string(stamp) + "_" + std::to_string(counter++) + ".bin");
		}

		~STempFile()
		{
			std::error_code ec;
			fs::remove(path, ec);
		}

		file::TFilePtr Open(file::enOpenFileMode mode, file::enAccesRights access, file::enShareMode share) const
		{
#ifdef _WIN32
			return file::CFileCreator::OpenFile(path.c_str(), mode, access, share, file::oftBinary);
#else
			return file::CFileCreator::OpenFileA(path.c_str(), mode, access, share, file::oftBinary);
#endif
		}

		file::TFilePtr Create() const
		{
			return Open(file::ofmCreateAlways, file::aeReadWrite, file::smNoMode);
		}

		file::TFilePtr OpenForRead() const
		{
			return Open(file::ofmOpenExisting, file::arRead, file::smRead);
		}
	};
}

TEST_CASE("CFileWriteStream writes and reports position and size", "[stream][filestream]")
{
	STempFile tmp;
	file::TFilePtr f = tmp.Create();
	CFileWriteStream ws(f);

	REQUIRE(ws.Pos() == 0);
	REQUIRE(ws.Size() == 0);

	const std::vector<byte_t> data = MakePattern(1000, 6);
	REQUIRE(ws.WriteBytes(data.data(), data.size()) == (std::streamsize)data.size());
	REQUIRE(ws.Pos() == 1000);
	REQUIRE(ws.Size() == 1000);

	ws.Write(uint32_t(0xDEADBEEF));
	REQUIRE(ws.Pos() == 1004);
	REQUIRE(ws.Size() == 1004);

	SECTION("Reset and Seek move the file pointer")
	{
		ws.Reset();
		REQUIRE(ws.Pos() == 0);

		ws.Seek(10, soFromBegin);
		REQUIRE(ws.Pos() == 10);
		ws.Seek(5, soFromCurrent);
		REQUIRE(ws.Pos() == 15);
		ws.Seek(0, soFromEnd);
		REQUIRE(ws.Pos() == 1004);
		REQUIRE(ws.SeekSafe(1, soFromBegin));
		REQUIRE(ws.Pos() == 1);
	}

	SECTION("overwriting in the middle does not change the size")
	{
		ws.Seek(100, soFromBegin);
		ws.Write(uint64_t(0));
		REQUIRE(ws.Pos() == 108);
		REQUIRE(ws.Size() == 1004);
	}

	SECTION("Close invalidates the underlying file")
	{
		REQUIRE(f->IsValid());
		ws.Close();
		REQUIRE_FALSE(f->IsValid());
	}
}

TEST_CASE("File stream round trip on one shared file handle", "[stream][filestream][roundtrip]")
{
	STempFile tmp;
	file::TFilePtr f = tmp.Create();

	const std::string text = "file stream text";
	const std::wstring wtext = L"wide file text";
	const std::vector<byte_t> blob = MakePattern(4096, 1);

	{
		CFileWriteStream ws(f);
		ws.Write(true);
		ws.Write(uint8_t(0x7E));
		ws.Write(int16_t(-30000));
		ws.Write(uint32_t(4000000000u));
		ws.Write(int64_t(-5000000000LL));
		ws.Write(2.75f);
		ws.Write(-1e100);
		ws.Write(text);
		ws.Write(wtext);
		ws.WriteBytes(blob.data(), blob.size());
		ws.Reset(); // rewind so the reader starts at the beginning
	}

	CFileReadStream rs(f);
	REQUIRE(rs.Pos() == 0);
	REQUIRE(rs.Size() == 1 + 1 + 2 + 4 + 8 + 4 + 8 + 4 + text.size() + 4 + wtext.size() * sizeof(wchar_t) + blob.size());

	REQUIRE(rs.ReadBool() == true);
	REQUIRE(rs.ReadByte() == 0x7E);
	REQUIRE(rs.Readint16() == -30000);
	REQUIRE(rs.ReadIntu32() == 4000000000u);
	REQUIRE(rs.ReadInt64() == -5000000000LL);
	REQUIRE(rs.ReadFloat() == 2.75f);
	REQUIRE(rs.ReadDouble() == -1e100);
	REQUIRE(rs.ReadAstr() == text);
	REQUIRE(rs.ReadWstr() == wtext);

	std::vector<byte_t> back(blob.size(), 0);
	REQUIRE(rs.ReadBytes(back.data(), back.size()) == (std::streamsize)back.size());
	REQUIRE(back == blob);
	REQUIRE(rs.Pos() == rs.Size());
}

TEST_CASE("CFileReadStream reads a file written and closed earlier", "[stream][filestream][roundtrip]")
{
	STempFile tmp;
	const std::vector<byte_t> data = MakePattern(777, 2);

	{
		CFileWriteStream ws(tmp.Create());
		for (byte_t b : data)
			ws.Write(b);
		ws.Close();
	}

	CFileReadStream rs(tmp.OpenForRead());
	REQUIRE(rs.Size() == data.size());

	std::vector<byte_t> back(data.size(), 0);
	rs.Read(back.data(), back.size());
	REQUIRE(back == data);

	SECTION("Seek lets the reader jump around")
	{
		rs.Seek(700, soFromBegin);
		REQUIRE(rs.Pos() == 700);
		REQUIRE(rs.ReadByte() == data[700]);

		rs.Seek(0, soFromEnd);
		REQUIRE(rs.Pos() == data.size());

		rs.Reset();
		REQUIRE(rs.ReadByte() == data[0]);
	}
}

TEST_CASE("CFileReadStream throws when the file has fewer bytes than requested", "[stream][filestream]")
{
	STempFile tmp;
	{
		CFileWriteStream ws(tmp.Create());
		ws.Write(uint32_t(1));
		ws.Close();
	}

	CFileReadStream rs(tmp.OpenForRead());
	REQUIRE(rs.Size() == 4);

	SECTION("reading past the end")
	{
		REQUIRE(rs.ReadIntu32() == 1);
		REQUIRE_THROWS_AS(rs.ReadByte(), CExcBase);
	}

	SECTION("a single read that is too large")
	{
		uint64_t v = 0;
		REQUIRE_THROWS_AS(rs.Read(v), CExcBase);
	}

	SECTION("ReadInverse checks the count as well")
	{
		byte_t buf[8];
		REQUIRE_THROWS_AS(rs.ReadInverse(buf, 8), CExcBase);
	}
}

TEST_CASE("Opening a missing file for reading throws", "[stream][filestream]")
{
	STempFile tmp; // never created on disk
	REQUIRE_THROWS_AS(tmp.OpenForRead(), std::exception);
}
