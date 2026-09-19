#include "TestCommon.h"
#include "UtilsLib/CRC.h"

using bptreedb::util::CCRC;

namespace
{
	uint32_t CrcOfString(const char* s)
	{
		return CCRC::Crc32(reinterpret_cast<const byte_t*>(s), (uint32_t)std::strlen(s));
	}
}

TEST_CASE("Crc32 known vectors (CRC-32/ISO-HDLC)", "[crc]")
{
	// Standard check value for CRC-32
	REQUIRE(CrcOfString("123456789") == 0xCBF43926u);
	REQUIRE(CrcOfString("a") == 0xE8B7BE43u);
	REQUIRE(CrcOfString("abc") == 0x352441C2u);
	REQUIRE(CrcOfString("The quick brown fox jumps over the lazy dog") == 0x414FA339u);
}

TEST_CASE("Crc32 of empty buffer is 0", "[crc]")
{
	byte_t dummy = 0;
	REQUIRE(CCRC::Crc32(&dummy, 0) == 0u);
}

TEST_CASE("Crc32 is sensitive to a single bit change", "[crc]")
{
	std::vector<byte_t> data(256);
	for (size_t i = 0; i < data.size(); ++i)
		data[i] = (byte_t)i;

	uint32_t base = CCRC::Crc32(data.data(), (uint32_t)data.size());

	for (size_t i = 0; i < data.size(); i += 37)
	{
		data[i] ^= 0x01;
		REQUIRE(CCRC::Crc32(data.data(), (uint32_t)data.size()) != base);
		data[i] ^= 0x01;
	}

	REQUIRE(CCRC::Crc32(data.data(), (uint32_t)data.size()) == base);
}

TEST_CASE("Crc32 is deterministic and depends on length", "[crc]")
{
	const char* s = "deterministic";
	uint32_t a = CrcOfString(s);
	uint32_t b = CrcOfString(s);
	REQUIRE(a == b);

	uint32_t shorter = CCRC::Crc32(reinterpret_cast<const byte_t*>(s), 5);
	REQUIRE(shorter != a);
}

TEST_CASE("Crc32 throws on null buffer", "[crc]")
{
	REQUIRE_THROWS_AS(CCRC::Crc32(nullptr, 10), CommonLib::CExcBase);
}
