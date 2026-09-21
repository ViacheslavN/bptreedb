#include "TestCommon.h"
#include "CommonLib/stream/MemoryStreamBuffer.h"

using namespace CommonLib;

TEST_CASE("CMemoryStreamBuffer default state", "[stream][membuffer]")
{
	CMemoryStreamBuffer buf;
	REQUIRE(buf.GetData() == nullptr);
	REQUIRE(buf.GetSize() == 0);
	REQUIRE_FALSE(buf.IsAttachedBuffer());
}

TEST_CASE("CMemoryStreamBuffer Create requires an allocator", "[stream][membuffer]")
{
	CMemoryStreamBuffer buf; // no allocator
	REQUIRE_THROWS_AS(buf.Create(16), CExcBase);
	REQUIRE(buf.GetData() == nullptr);
	REQUIRE(buf.GetSize() == 0);
}

TEST_CASE("CMemoryStreamBuffer Create / destroy uses the allocator", "[stream][membuffer][alloc]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);

	{
		CMemoryStreamBuffer buf(alloc);
		buf.Create(64);

		REQUIRE(buf.GetData() != nullptr);
		REQUIRE(buf.GetSize() == 64);
		REQUIRE_FALSE(buf.IsAttachedBuffer());
		REQUIRE(alloc->GetAllocCount() == 1);
		REQUIRE(alloc->GetCurrentMemoryBalance() == 64);

		SECTION("Create again frees the previous block")
		{
			buf.Create(128);
			REQUIRE(buf.GetSize() == 128);
			REQUIRE(alloc->GetAllocCount() == 2);
			REQUIRE(alloc->GetFreeCount() == 1);
			REQUIRE(alloc->GetCurrentMemoryBalance() == 128);
		}

		SECTION("Close frees the owned block and zeroes the size")
		{
			buf.Close();
			REQUIRE(buf.GetSize() == 0);
			REQUIRE(buf.GetData() == nullptr);
			REQUIRE(alloc->GetFreeCount() == 1);
			REQUIRE(alloc->GetCurrentMemoryBalance() == 0);
		}
	}

	// whatever path was taken, the destructor must leave nothing behind
	REQUIRE(alloc->GetCurrentMemoryBalance() == 0);
	REQUIRE(alloc->GetAllocCount() == alloc->GetFreeCount());
}

TEST_CASE("CMemoryStreamBuffer AttachBuffer without copy", "[stream][membuffer]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	std::vector<byte_t> external = test_utils::MakePattern(32);

	{
		CMemoryStreamBuffer buf(alloc);
		buf.AttachBuffer(external.data(), (uint32_t)external.size(), false);

		REQUIRE(buf.IsAttachedBuffer());
		REQUIRE(buf.GetData() == external.data());
		REQUIRE(buf.GetSize() == external.size());

		// writes through the buffer land in the external storage
		buf.GetData()[0] = 0xEE;
		REQUIRE(external[0] == 0xEE);

		SECTION("Close on an attached buffer does not free it")
		{
			buf.Close();
			REQUIRE(buf.GetSize() == 0);
			REQUIRE(alloc->GetFreeCount() == 0);
		}
	}

	// nothing was ever allocated or freed through the allocator
	REQUIRE(alloc->GetAllocCount() == 0);
	REQUIRE(alloc->GetFreeCount() == 0);
	REQUIRE(external[1] == test_utils::MakePattern(32)[1]);
}

TEST_CASE("CMemoryStreamBuffer AttachBuffer with copy", "[stream][membuffer]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	std::vector<byte_t> external = test_utils::MakePattern(32, 3);

	{
		CMemoryStreamBuffer buf(alloc);
		buf.AttachBuffer(external.data(), (uint32_t)external.size(), true);

		REQUIRE_FALSE(buf.IsAttachedBuffer());
		REQUIRE(buf.GetData() != external.data());
		REQUIRE(buf.GetSize() == external.size());
		REQUIRE(test_utils::SameBytes(buf.GetData(), external));
		REQUIRE(alloc->GetAllocCount() == 1);

		// modifying the copy leaves the original alone
		buf.GetData()[5] = byte_t(~external[5]);
		REQUIRE(external[5] != buf.GetData()[5]);
	}

	REQUIRE(alloc->GetCurrentMemoryBalance() == 0);
}

TEST_CASE("CMemoryStreamBuffer DeattachBuffer hands ownership to the caller", "[stream][membuffer][alloc]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);
	byte_t* detached = nullptr;

	{
		CMemoryStreamBuffer buf(alloc);
		buf.Create(16);
		byte_t* created = buf.GetData();

		detached = buf.DeattachBuffer();

		REQUIRE(detached == created);
		REQUIRE(buf.GetData() == nullptr);
		REQUIRE(buf.GetSize() == 0);
		REQUIRE_FALSE(buf.IsAttachedBuffer());
	}

	// the buffer's destructor must not have freed it
	REQUIRE(alloc->GetFreeCount() == 0);
	REQUIRE(alloc->GetCurrentMemoryBalance() == 16);

	alloc->Free(detached);
	REQUIRE(alloc->GetCurrentMemoryBalance() == 0);
}

TEST_CASE("CMemoryStreamBuffer CreateBuffer shares the allocator", "[stream][membuffer][alloc]")
{
	IAllocPtr alloc = IAlloc::CreateSimpleAlloc(true /*debug: track counters*/);

	{
		CMemoryStreamBuffer buf(alloc);
		IMemStreamBufferPtr other = buf.CreateBuffer();

		REQUIRE(other != nullptr);
		REQUIRE(other->GetData() == nullptr);
		REQUIRE(other->GetSize() == 0);

		other->Create(40);
		REQUIRE(other->GetSize() == 40);
		REQUIRE(alloc->GetAllocCount() == 1);
	}

	REQUIRE(alloc->GetCurrentMemoryBalance() == 0);
}
