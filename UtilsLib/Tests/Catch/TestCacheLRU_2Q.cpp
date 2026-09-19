#include "TestCommon.h"
#include "UtilsLib/CacheLRU_2Q.h"

using bptreedb::utils::TCacheLRU_2Q;

namespace
{
	const int kNull = -1;

	typedef TCacheLRU_2Q<int, int> TIntCache;

	// Free checker that reports objects as pinned while their value is in the set
	struct SPinnedChecker
	{
		static std::set<int>& Pinned()
		{
			static std::set<int> s;
			return s;
		}

		bool IsFree(int& obj)
		{
			return Pinned().count(obj) == 0;
		}
	};

	// Collect keys in map (sorted) order
	template<class TCache>
	std::vector<int> Keys(TCache& cache)
	{
		std::vector<int> out;
		for (auto it = cache.Begin(); !it.IsNull(); it.Next())
			out.push_back(it.Key());
		return out;
	}
}

TEST_CASE("Cache is empty on construction", "[cache]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	REQUIRE(cache.Size() == 0);
	REQUIRE(cache.Begin().IsNull());
	REQUIRE(cache.RemoveBack() == kNull);
	REQUIRE(cache.GetElem(1) == kNull);
	REQUIRE(cache.Remove(1) == kNull);
}

TEST_CASE("AddElem / GetElem / Size", "[cache]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int v10 = 100, v20 = 200, v30 = 300;

	cache.AddElem(10, v10);
	cache.AddElem(20, v20);
	cache.AddElem(30, v30, false); // straight into the top (hot) queue

	REQUIRE(cache.Size() == 3);
	REQUIRE(cache.GetElem(10) == 100);
	REQUIRE(cache.GetElem(20) == 200);
	REQUIRE(cache.GetElem(30) == 300);
	REQUIRE(cache.GetElem(40) == kNull);
	REQUIRE(Keys(cache) == std::vector<int>{ 10, 20, 30 });
}

TEST_CASE("GetElem returns a reference to the stored object", "[cache]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int v = 5;
	cache.AddElem(1, v);

	cache.GetElem(1) = 77;
	REQUIRE(cache.GetElem(1) == 77);
	REQUIRE(cache.GetElem(1, true) == 77);
}

TEST_CASE("AddElem with a duplicate key throws", "[cache]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int v = 1;
	cache.AddElem(1, v);
	REQUIRE_THROWS_AS(cache.AddElem(1, v), CommonLib::CExcBase);
	REQUIRE(cache.Size() == 1);
}

TEST_CASE("AddElemIfNoExists ignores duplicates", "[cache]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2;
	cache.AddElemIfNoExists(1, a);
	cache.AddElemIfNoExists(1, b);
	REQUIRE(cache.Size() == 1);
	REQUIRE(cache.GetElem(1) == 1);

	cache.AddElemIfNoExists(2, b);
	REQUIRE(cache.Size() == 2);
}

TEST_CASE("Remove by key", "[cache]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2, c = 3;
	cache.AddElem(1, a);
	cache.AddElem(2, b);
	cache.AddElem(3, c, false);

	SECTION("remove from back queue")
	{
		REQUIRE(cache.Remove(2) == 2);
		REQUIRE(cache.Size() == 2);
		REQUIRE(cache.GetElem(2) == kNull);
		REQUIRE(Keys(cache) == std::vector<int>{ 1, 3 });
	}

	SECTION("remove from top queue")
	{
		REQUIRE(cache.Remove(3) == 3);
		REQUIRE(cache.Size() == 2);
		REQUIRE(cache.GetElem(3) == kNull);
	}

	SECTION("remove missing key returns null object")
	{
		REQUIRE(cache.Remove(99) == kNull);
		REQUIRE(cache.Size() == 3);
	}

	SECTION("remove all")
	{
		cache.Remove(1);
		cache.Remove(2);
		cache.Remove(3);
		REQUIRE(cache.Size() == 0);
		REQUIRE(cache.RemoveBack() == kNull);
	}
}

TEST_CASE("RemoveBack evicts the oldest element of the back (probation) queue first", "[cache][2q]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2, c = 3;
	cache.AddElem(1, a);
	cache.AddElem(2, b);
	cache.AddElem(3, c);

	REQUIRE(cache.RemoveBack() == 1);
	REQUIRE(cache.Size() == 2);
	REQUIRE(cache.RemoveBack() == 2);
	REQUIRE(cache.RemoveBack() == 3);
	REQUIRE(cache.Size() == 0);
	REQUIRE(cache.RemoveBack() == kNull);
}

TEST_CASE("GetElem refreshes recency inside the back queue", "[cache][2q]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2, c = 3;
	cache.AddElem(1, a);
	cache.AddElem(2, b);
	cache.AddElem(3, c);

	// 1 is the oldest; one access moves it to the top of the back queue
	REQUIRE(cache.GetElem(1) == 1);

	REQUIRE(cache.RemoveBack() == 2);
	REQUIRE(cache.RemoveBack() == 3);
	REQUIRE(cache.RemoveBack() == 1);
}

TEST_CASE("GetElem with bNotMove does not change recency", "[cache][2q]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2, c = 3;
	cache.AddElem(1, a);
	cache.AddElem(2, b);
	cache.AddElem(3, c);

	REQUIRE(cache.GetElem(1, true) == 1);
	REQUIRE(cache.GetElem(1, true) == 1);

	REQUIRE(cache.RemoveBack() == 1);
}

TEST_CASE("Element accessed while at the top of the back queue is promoted to the top queue", "[cache][2q]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2, c = 3;
	cache.AddElem(1, a);
	cache.AddElem(2, b);
	cache.AddElem(3, c);

	// 3 is already the most recent in the back queue -> promoted to the hot queue
	REQUIRE(cache.GetElem(3) == 3);

	// Back queue now holds 2 (newest), 1 (oldest); hot queue holds 3
	REQUIRE(cache.RemoveBack() == 1);
	// Evicting from the back queue demotes the oldest hot element (3) into the back queue
	REQUIRE(cache.RemoveBack() == 2);
	REQUIRE(cache.RemoveBack() == 3);
	REQUIRE(cache.RemoveBack() == kNull);
}

TEST_CASE("Hot queue elements are evicted only when the back queue is empty", "[cache][2q]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2, c = 3, d = 4;
	cache.AddElem(1, a, false); // hot
	cache.AddElem(2, b, false); // hot
	cache.AddElem(3, c);        // back
	cache.AddElem(4, d);        // back

	// Hot elements added with bAddBack=false go to the *back* of the hot queue,
	// i.e. they rank as least recently used: hot order (newest->oldest) is 1, 2.
	REQUIRE(cache.RemoveBack() == 3); // back queue first; 2 (LRU of hot) demoted to back
	REQUIRE(cache.RemoveBack() == 4); // 4 is older than the demoted 2; 1 demoted
	REQUIRE(cache.RemoveBack() == 2);
	REQUIRE(cache.RemoveBack() == 1);
	REQUIRE(cache.RemoveBack() == kNull);
	REQUIRE(cache.Size() == 0);
}

TEST_CASE("Hot queue keeps LRU order on access", "[cache][2q]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2, c = 3;
	cache.AddElem(1, a, false);
	cache.AddElem(2, b, false);
	cache.AddElem(3, c, false);

	// PushBack order: 1 (top/newest) .. 3 (back/oldest) -> touching 3 makes it newest
	REQUIRE(cache.GetElem(3) == 3);

	REQUIRE(cache.RemoveBack() == 2);
	REQUIRE(cache.RemoveBack() == 1);
	REQUIRE(cache.RemoveBack() == 3);
}

TEST_CASE("Free checker prevents eviction of pinned objects", "[cache][2q]")
{
	typedef TCacheLRU_2Q<int, int, SPinnedChecker> TPinnedCache;
	SPinnedChecker::Pinned().clear();

	TPinnedCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2, c = 3;
	cache.AddElem(1, a);
	cache.AddElem(2, b);
	cache.AddElem(3, c);

	SECTION("pinned oldest element is skipped")
	{
		SPinnedChecker::Pinned().insert(1);
		REQUIRE(cache.RemoveBack() == 2);
		REQUIRE(cache.Size() == 2);
		REQUIRE(cache.GetElem(1, true) == 1);
	}

	SECTION("everything pinned - nothing evicted")
	{
		SPinnedChecker::Pinned() = { 1, 2, 3 };
		REQUIRE(cache.RemoveBack() == kNull);
		REQUIRE(cache.Size() == 3);
	}

	SECTION("unpinning makes the element evictable again")
	{
		SPinnedChecker::Pinned() = { 1, 2, 3 };
		REQUIRE(cache.RemoveBack() == kNull);
		SPinnedChecker::Pinned().erase(2);
		REQUIRE(cache.RemoveBack() == 2);
		REQUIRE(cache.Size() == 2);
	}

	SPinnedChecker::Pinned().clear();
}

TEST_CASE("Clear empties the cache and it stays usable", "[cache]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	int a = 1, b = 2;
	cache.AddElem(1, a);
	cache.AddElem(2, b, false);

	cache.Clear();
	REQUIRE(cache.Size() == 0);
	REQUIRE(cache.GetElem(1) == kNull);
	REQUIRE(cache.RemoveBack() == kNull);

	cache.AddElem(1, a);
	REQUIRE(cache.Size() == 1);
	REQUIRE(cache.GetElem(1) == 1);
	REQUIRE(cache.RemoveBack() == 1);
}

TEST_CASE("Iterator visits every element", "[cache]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	for (int i = 0; i < 10; ++i)
	{
		int v = i * 10;
		cache.AddElem(i, v, (i % 2) == 0);
	}

	int count = 0;
	for (auto it = cache.Begin(); !it.IsNull(); it.Next())
	{
		REQUIRE(it.Object() == it.Key() * 10);
		++count;
	}
	REQUIRE(count == 10);
}

TEST_CASE("Cache with shared_ptr objects", "[cache]")
{
	typedef std::shared_ptr<std::string> TStrPtr;
	TCacheLRU_2Q<int64_t, TStrPtr> cache;

	TStrPtr p1 = std::make_shared<std::string>("one");
	TStrPtr p2 = std::make_shared<std::string>("two");
	cache.AddElem(1, p1);
	cache.AddElem(2, p2);

	REQUIRE(p1.use_count() == 2);
	REQUIRE(*cache.GetElem(1) == "one");
	REQUIRE(cache.GetElem(3) == nullptr);

	TStrPtr evicted = cache.RemoveBack();
	REQUIRE(evicted == p1);
	REQUIRE(cache.Size() == 1);

	cache.Clear();
	REQUIRE(p1.use_count() == 2); // p1 + evicted
	REQUIRE(p2.use_count() == 1);
}

TEST_CASE("Cache releases all node memory through the allocator", "[cache][alloc]")
{
	CommonLib::IAllocPtr pAlloc = CommonLib::IAlloc::CreateSimpleAlloc(true);

	{
		TCacheLRU_2Q<int, int> cache(pAlloc, kNull);
		for (int i = 0; i < 50; ++i)
		{
			int v = i;
			cache.AddElem(i, v, (i % 3) != 0);
		}
		for (int i = 0; i < 50; i += 5)
			cache.GetElem(i);
		for (int i = 0; i < 10; ++i)
			cache.RemoveBack();
		cache.Remove(49);
		REQUIRE(cache.Size() == 39);
	}

	REQUIRE(pAlloc->GetAllocCount() == 50);
	REQUIRE(pAlloc->GetAllocCount() == pAlloc->GetFreeCount());
	REQUIRE(pAlloc->GetCurrentMemoryBalance() == 0);
}

TEST_CASE("Stress: interleaved add / get / evict keeps Size consistent", "[cache][stress]")
{
	TIntCache cache(CommonLib::IAllocPtr(), kNull);
	const int kCapacity = 64;
	std::set<int> present;

	uint32_t seed = 12345;
	auto next = [&seed]() { seed = seed * 1103515245u + 12345u; return (seed >> 8); };

	for (int step = 0; step < 20000; ++step)
	{
		int key = (int)(next() % 200);
		switch (next() % 3)
		{
		case 0:
		{
			int v = key;
			cache.AddElemIfNoExists(key, v);
			present.insert(key);
			while (cache.Size() > (uint32_t)kCapacity)
			{
				int evicted = cache.RemoveBack();
				REQUIRE(evicted != kNull);
				present.erase(evicted);
			}
			break;
		}
		case 1:
		{
			int& ref = cache.GetElem(key);
			if (present.count(key))
				REQUIRE(ref == key);
			else
				REQUIRE(ref == kNull);
			break;
		}
		case 2:
		{
			int removed = cache.Remove(key);
			if (present.count(key))
			{
				REQUIRE(removed == key);
				present.erase(key);
			}
			else
				REQUIRE(removed == kNull);
			break;
		}
		}
		REQUIRE(cache.Size() == present.size());
	}
}
