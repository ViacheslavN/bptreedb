#include "TestCommon.h"

using namespace CommonLib;
using test_utils::MakeBlob;

namespace
{
	// Checks all six relational operators plus Compare() for lhs < rhs.
	void ExpectLess(const CVariant& lhs, const CVariant& rhs)
	{
		REQUIRE(lhs.Compare(rhs) < 0);
		REQUIRE(rhs.Compare(lhs) > 0);
		REQUIRE(lhs < rhs);
		REQUIRE(lhs <= rhs);
		REQUIRE_FALSE(lhs > rhs);
		REQUIRE_FALSE(lhs >= rhs);
		REQUIRE_FALSE(lhs == rhs);
		REQUIRE(lhs != rhs);

		REQUIRE(rhs > lhs);
		REQUIRE(rhs >= lhs);
		REQUIRE_FALSE(rhs < lhs);
		REQUIRE_FALSE(rhs <= lhs);
	}

	void ExpectEqual(const CVariant& lhs, const CVariant& rhs)
	{
		REQUIRE(lhs.Compare(rhs) == 0);
		REQUIRE(rhs.Compare(lhs) == 0);
		REQUIRE(lhs == rhs);
		REQUIRE_FALSE(lhs != rhs);
		REQUIRE(lhs <= rhs);
		REQUIRE(lhs >= rhs);
		REQUIRE_FALSE(lhs < rhs);
		REQUIRE_FALSE(lhs > rhs);
	}
}

TEST_CASE("Comparison of variants holding the same type", "[variant][compare]")
{
	SECTION("integers")
	{
		ExpectLess(CVariant(int32_t(-5)), CVariant(int32_t(5)));
		ExpectLess(CVariant(uint64_t(1)), CVariant(std::numeric_limits<uint64_t>::max()));
		ExpectLess(CVariant(int8_t(-128)), CVariant(int8_t(127)));
		ExpectEqual(CVariant(int32_t(42)), CVariant(int32_t(42)));
		ExpectEqual(CVariant(uint16_t(0)), CVariant(uint16_t(0)));
	}

	SECTION("bool")
	{
		ExpectLess(CVariant(false), CVariant(true));
		ExpectEqual(CVariant(true), CVariant(true));
	}

	SECTION("floating point")
	{
		ExpectLess(CVariant(1.5f), CVariant(2.5f));
		ExpectLess(CVariant(-0.001), CVariant(0.0));
		ExpectEqual(CVariant(0.25), CVariant(0.25));
	}

	SECTION("strings compare lexicographically")
	{
		ExpectLess(CVariant(std::string("apple")), CVariant(std::string("banana")));
		ExpectLess(CVariant(std::string("")), CVariant(std::string("a")));
		ExpectLess(CVariant(std::string("ab")), CVariant(std::string("abc")));
		ExpectEqual(CVariant(std::string("same")), CVariant(std::string("same")));

		ExpectLess(CVariant(std::wstring(L"a")), CVariant(std::wstring(L"b")));
		ExpectEqual(CVariant(std::wstring(L"w")), CVariant(std::wstring(L"w")));
	}

	SECTION("guids")
	{
		const CGuid a = CGuid::CreateNull();
		const CGuid b = CGuid::CreateNew();
		REQUIRE(a != b);
		ExpectEqual(CVariant(a), CVariant(a));
		if (a < b)
			ExpectLess(CVariant(a), CVariant(b));
		else
			ExpectLess(CVariant(b), CVariant(a));
	}

	SECTION("empty variants are all equal")
	{
		ExpectEqual(CVariant(), CVariant());
		ExpectEqual(CVariant(), CVariant(CEmptyVariant()));
	}
}

TEST_CASE("Blob variants compare by pointer identity", "[variant][compare][blob]")
{
	Data::CBlobPtr a = MakeBlob({ 1, 2, 3 });
	Data::CBlobPtr b = MakeBlob({ 1, 2, 3 }); // same content, different object

	ExpectEqual(CVariant(a), CVariant(a));
	REQUIRE(CVariant(a) != CVariant(b));
	REQUIRE(CVariant(a).Compare(CVariant(b)) != 0);

	// ordering is consistent with the raw pointers
	if (a < b)
		ExpectLess(CVariant(a), CVariant(b));
	else
		ExpectLess(CVariant(b), CVariant(a));

	ExpectEqual(CVariant(Data::CBlobPtr()), CVariant(Data::CBlobPtr()));
}

TEST_CASE("Variants of different types are ordered by type id, never equal", "[variant][compare][typeid]")
{
	// declaration order in VariantTypeList.h: empty < bool < int8 < ... < double < guid < astr < wstr < shape < blob
	ExpectLess(CVariant(), CVariant(false));
	ExpectLess(CVariant(true), CVariant(int8_t(0)));
	ExpectLess(CVariant(int32_t(1000)), CVariant(int64_t(-1000))); // type wins over value
	ExpectLess(CVariant(double(1e9)), CVariant(CGuid::CreateNull()));
	ExpectLess(CVariant(CGuid::CreateNew()), CVariant(std::string("")));
	ExpectLess(CVariant(std::string("zzz")), CVariant(std::wstring(L"aaa")));
	ExpectLess(CVariant(std::wstring(L"")), CVariant(IGeoShapePtr()));
	ExpectLess(CVariant(IGeoShapePtr()), CVariant(MakeBlob({})));

	SECTION("Compare() returns the type id difference")
	{
		REQUIRE(CVariant(int32_t(0)).Compare(CVariant(std::string())) == int(varType_int32_t) - int(varType_astr_t));
		REQUIRE(CVariant(std::string()).Compare(CVariant(int32_t(0))) == int(varType_astr_t) - int(varType_int32_t));
	}

	SECTION("numerically equal values of different width are not equal")
	{
		REQUIRE(CVariant(int32_t(5)) != CVariant(int64_t(5)));
		REQUIRE(CVariant(uint8_t(1)) != CVariant(true));
		REQUIRE(CVariant(1.0f) != CVariant(1.0));
	}
}

TEST_CASE("Variants sort with std::sort", "[variant][compare]")
{
	std::vector<CVariant> values;
	values.push_back(CVariant(std::string("b")));
	values.push_back(CVariant(int32_t(30)));
	values.push_back(CVariant());
	values.push_back(CVariant(std::string("a")));
	values.push_back(CVariant(int32_t(10)));
	values.push_back(CVariant(double(0.5)));
	values.push_back(CVariant(int32_t(20)));

	std::sort(values.begin(), values.end());

	REQUIRE(values[0].IsNull());
	REQUIRE(values[1].Get<int32_t>() == 10);
	REQUIRE(values[2].Get<int32_t>() == 20);
	REQUIRE(values[3].Get<int32_t>() == 30);
	REQUIRE(values[4].Get<double>() == 0.5);
	REQUIRE(values[5].Get<std::string>() == "a");
	REQUIRE(values[6].Get<std::string>() == "b");

	// operator< is a strict weak ordering: the sorted range is non-decreasing
	for (size_t i = 1; i < values.size(); ++i)
	{
		REQUIRE_FALSE(values[i] < values[i - 1]);
		REQUIRE(values[i - 1] <= values[i]);
	}
}

TEST_CASE("Variants can key a std::map", "[variant][compare]")
{
	std::map<CVariant, int> byKey;
	byKey[CVariant(int32_t(1))] = 1;
	byKey[CVariant(std::string("one"))] = 2;
	byKey[CVariant()] = 3;
	byKey[CVariant(int32_t(1))] = 11; // overwrite, same key

	REQUIRE(byKey.size() == 3);
	REQUIRE(byKey.at(CVariant(int32_t(1))) == 11);
	REQUIRE(byKey.at(CVariant(std::string("one"))) == 2);
	REQUIRE(byKey.at(CVariant()) == 3);
	REQUIRE(byKey.find(CVariant(int64_t(1))) == byKey.end()); // different type, different key
}
