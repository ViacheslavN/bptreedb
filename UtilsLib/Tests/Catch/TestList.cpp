#include "TestCommon.h"
#include "UtilsLib/List.h"

using bptreedb::utils::TList;

namespace
{
	// Walk Begin() -> Back() through m_pNext
	template<class T>
	std::vector<T> ToVectorForward(TList<T>& list)
	{
		std::vector<T> out;
		for (auto it = list.Begin(); !it.IsNull(); it.Next())
			out.push_back(it.Value());
		return out;
	}

	// Walk Back() -> Begin() through m_pPrev
	template<class T>
	std::vector<T> ToVectorBackward(TList<T>& list)
	{
		std::vector<T> out;
		for (auto it = list.Back(); !it.IsNull(); it.Back())
			out.push_back(it.Value());
		return out;
	}
}

TEST_CASE("TList is empty on construction", "[list]")
{
	TList<int> list;
	REQUIRE(list.Size() == 0);
	REQUIRE(list.Begin().IsNull());
	REQUIRE(list.Back().IsNull());
}

TEST_CASE("TList PushBack keeps insertion order", "[list]")
{
	TList<int> list;
	list.PushBack(1);
	list.PushBack(2);
	list.PushBack(3);

	REQUIRE(list.Size() == 3);
	REQUIRE(ToVectorForward(list) == std::vector<int>{ 1, 2, 3 });
	REQUIRE(ToVectorBackward(list) == std::vector<int>{ 3, 2, 1 });
	REQUIRE(list.Begin().Value() == 1);
	REQUIRE(list.Back().Value() == 3);
}

TEST_CASE("TList PushTop prepends", "[list]")
{
	TList<int> list;
	list.PushTop(1);
	list.PushTop(2);
	list.PushTop(3);

	REQUIRE(list.Size() == 3);
	REQUIRE(ToVectorForward(list) == std::vector<int>{ 3, 2, 1 });
	REQUIRE(ToVectorBackward(list) == std::vector<int>{ 1, 2, 3 });
	REQUIRE(list.IsTop(list.Begin().Node()));
	REQUIRE_FALSE(list.IsTop(list.Back().Node()));
}

TEST_CASE("TList mixed PushTop / PushBack", "[list]")
{
	TList<int> list;
	list.PushBack(2);
	list.PushTop(1);
	list.PushBack(3);
	list.PushTop(0);

	REQUIRE(list.Size() == 4);
	REQUIRE(ToVectorForward(list) == std::vector<int>{ 0, 1, 2, 3 });
	REQUIRE(ToVectorBackward(list) == std::vector<int>{ 3, 2, 1, 0 });
}

TEST_CASE("TList Remove", "[list]")
{
	TList<int> list;
	for (int i = 1; i <= 5; ++i)
		list.PushBack(i);

	SECTION("remove middle node returns iterator to next")
	{
		auto it = list.Begin();
		it.Next(); it.Next(); // -> 3
		auto next = list.Remove(it);
		REQUIRE(next.Value() == 4);
		REQUIRE(list.Size() == 4);
		REQUIRE(ToVectorForward(list) == std::vector<int>{ 1, 2, 4, 5 });
		REQUIRE(ToVectorBackward(list) == std::vector<int>{ 5, 4, 2, 1 });
	}

	SECTION("remove first node updates Begin")
	{
		auto next = list.Remove(list.Begin());
		REQUIRE(next.Value() == 2);
		REQUIRE(list.Begin().Value() == 2);
		REQUIRE(list.Begin().Node()->m_pPrev == nullptr);
		REQUIRE(list.Size() == 4);
	}

	SECTION("remove last node updates Back")
	{
		auto next = list.Remove(list.Back());
		REQUIRE(next.IsNull());
		REQUIRE(list.Back().Value() == 4);
		REQUIRE(list.Back().Node()->m_pNext == nullptr);
		REQUIRE(list.Size() == 4);
	}

	SECTION("remove every node leaves an empty list")
	{
		while (!list.Begin().IsNull())
			list.Remove(list.Begin());

		REQUIRE(list.Size() == 0);
		REQUIRE(list.Begin().IsNull());
		REQUIRE(list.Back().IsNull());
	}

	SECTION("remove null node is a no-op")
	{
		auto it = list.Remove((TList<int>::TNode*)nullptr);
		REQUIRE(it.IsNull());
		REQUIRE(list.Size() == 5);
	}
}

TEST_CASE("TList Remove without delete lets the node be re-inserted", "[list]")
{
	TList<int> list;
	list.PushBack(1);
	list.PushBack(2);
	list.PushBack(3);

	auto it = list.Begin();
	it.Next(); // -> 2
	TList<int>::TNode* pNode = it.Node();

	list.Remove(pNode, false);
	REQUIRE(list.Size() == 2);
	REQUIRE(pNode->m_pNext == nullptr);
	REQUIRE(pNode->m_pPrev == nullptr);
	REQUIRE(ToVectorForward(list) == std::vector<int>{ 1, 3 });

	list.PushTop(pNode);
	REQUIRE(list.Size() == 3);
	REQUIRE(ToVectorForward(list) == std::vector<int>{ 2, 1, 3 });
	REQUIRE(ToVectorBackward(list) == std::vector<int>{ 3, 1, 2 });
}

TEST_CASE("TList PushTop / PushBack of a node already at that end is a no-op", "[list]")
{
	TList<int> list;
	list.PushBack(1);
	list.PushBack(2);

	list.PushTop(list.Begin().Node());
	list.PushBack(list.Back().Node());

	REQUIRE(list.Size() == 2);
	REQUIRE(ToVectorForward(list) == std::vector<int>{ 1, 2 });
}

TEST_CASE("TList Insert after a node", "[list]")
{
	TList<int> list;
	list.PushBack(1);
	list.PushBack(3);

	SECTION("insert in the middle")
	{
		auto it = list.Insert(list.Begin(), 2);
		REQUIRE(it.Value() == 2);
		REQUIRE(list.Size() == 3);
		REQUIRE(ToVectorForward(list) == std::vector<int>{ 1, 2, 3 });
		REQUIRE(ToVectorBackward(list) == std::vector<int>{ 3, 2, 1 });
	}

	SECTION("insert after the last node becomes the new Back")
	{
		auto it = list.Insert(list.Back(), 4);
		REQUIRE(it.Value() == 4);
		REQUIRE(list.Size() == 3);
		REQUIRE(list.Back().Value() == 4);
		REQUIRE(list.Begin().Value() == 1);
		REQUIRE(ToVectorForward(list) == std::vector<int>{ 1, 3, 4 });
	}

	SECTION("insert with a null iterator does nothing")
	{
		auto it = list.Insert(TList<int>::iterator(nullptr), 9);
		REQUIRE(it.IsNull());
		REQUIRE(list.Size() == 2);
	}
}

TEST_CASE("TList Clear", "[list]")
{
	TList<int> list;

	SECTION("clear on empty list")
	{
		list.Clear();
		REQUIRE(list.Size() == 0);
	}

	SECTION("clear multi-element list")
	{
		for (int i = 0; i < 10; ++i)
			list.PushBack(i);
		list.Clear();
		REQUIRE(list.Size() == 0);
		REQUIRE(list.Begin().IsNull());
		REQUIRE(list.Back().IsNull());
	}

	SECTION("clear single-element list")
	{
		list.PushBack(42);
		list.Clear();
		REQUIRE(list.Begin().IsNull());
		REQUIRE(list.Back().IsNull());
		REQUIRE(list.Size() == 0);
	}

	SECTION("list is usable after clear")
	{
		list.PushBack(1);
		list.PushBack(2);
		list.Clear();
		list.PushBack(3);
		REQUIRE(list.Size() == 1);
		REQUIRE(list.Begin().Value() == 3);
		REQUIRE(list.Back().Value() == 3);
	}
}

TEST_CASE("TList iterator throws on null node access", "[list]")
{
	TList<int>::iterator it;
	REQUIRE(it.IsNull());
	REQUIRE_THROWS_AS(it.Next(), CommonLib::CExcBase);
	REQUIRE_THROWS_AS(it.Back(), CommonLib::CExcBase);
	REQUIRE_THROWS_AS(it.Value(), CommonLib::CExcBase);
}

TEST_CASE("TList releases all memory through the allocator", "[list][alloc]")
{
	CommonLib::IAllocPtr pAlloc = CommonLib::IAlloc::CreateSimpleAlloc(true);

	{
		TList<std::string> list(pAlloc);
		for (int i = 0; i < 100; ++i)
			list.PushBack("item " + std::to_string(i));
		list.Remove(list.Begin());
		list.Remove(list.Back());
		REQUIRE(list.Size() == 98);
		REQUIRE(pAlloc->GetAllocCount() == 100);
		REQUIRE(pAlloc->GetFreeCount() == 2);
	}

	REQUIRE(pAlloc->GetAllocCount() == pAlloc->GetFreeCount());
	REQUIRE(pAlloc->GetCurrentMemoryBalance() == 0);
}

TEST_CASE("TList with a non-trivial value type", "[list]")
{
	struct SVal
	{
		int a;
		std::string s;
	};

	TList<SVal> list;
	list.PushBack({ 1, "one" });
	list.PushBack({ 2, "two" });

	auto it = list.Begin();
	REQUIRE(it.Value().a == 1);
	REQUIRE(it.Value().s == "one");
	it.Next();
	REQUIRE(it.Value().a == 2);
	REQUIRE(it.Value().s == "two");

	it.Value().s = "TWO";
	REQUIRE(list.Back().Value().s == "TWO");
}
