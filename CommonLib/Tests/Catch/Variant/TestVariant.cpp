#include "TestCommon.h"

using namespace CommonLib;
using test_utils::MakeBlob;

// ---------------------------------------------------------------------------
// Empty variant
// ---------------------------------------------------------------------------

TEST_CASE("Default constructed CVariant is empty", "[variant]")
{
	CVariant v;

	REQUIRE(v.IsNull());
	REQUIRE(v.IsType<CEmptyVariant>());
	REQUIRE(v.GetTypeID() == varType_CEmptyVariant);
	REQUIRE_FALSE(v.IsType<int32_t>());
	REQUIRE_FALSE(v.IsType<std::string>());

	REQUIRE(v.GetPtr<int32_t>() == nullptr);
	REQUIRE_THROWS_AS(v.Get<int32_t>(), CExcBase);
	REQUIRE_THROWS_AS(v.Get<std::string>(), CExcBase);

	int32_t out = 5;
	REQUIRE_THROWS_AS(v.GetVal(out), CExcBase);
	REQUIRE(out == 5); // untouched after a failed GetVal

	// the empty payload itself is reachable
	REQUIRE_NOTHROW(v.Get<CEmptyVariant>());
}

TEST_CASE("Type ids follow the declaration order of VariantTypeList.h", "[variant][typeid]")
{
	REQUIRE(varType_CEmptyVariant == 0);
	REQUIRE(varType_bool == 1);
	REQUIRE(varType_int8_t == 2);
	REQUIRE(varType_uint8_t == 3);
	REQUIRE(varType_int16_t == 4);
	REQUIRE(varType_uint16_t == 5);
	REQUIRE(varType_int32_t == 6);
	REQUIRE(varType_uint32_t == 7);
	REQUIRE(varType_int64_t == 8);
	REQUIRE(varType_uint64_t == 9);
	REQUIRE(varType_float == 10);
	REQUIRE(varType_double == 11);
	REQUIRE(varType_CGuid == 12);
	REQUIRE(varType_astr_t == 13);
	REQUIRE(varType_wstr_t == 14);
	REQUIRE(varType_IGeoShapePtr == 15);
	REQUIRE(varType_CBlobPtr_t == 16);

	REQUIRE(int(type2int<int32_t>::typeId) == int(varType_int32_t));
	REQUIRE(int(type2int<std::string>::typeId) == int(varType_astr_t));
	REQUIRE(int(type2int<Data::CBlobPtr>::typeId) == int(varType_CBlobPtr_t));
}

// ---------------------------------------------------------------------------
// Construction from every supported type
// ---------------------------------------------------------------------------

namespace
{
	template<class T>
	void CheckHolds(const T& value, eDataTypes expectedId)
	{
		CVariant v(value);

		REQUIRE_FALSE(v.IsNull());
		REQUIRE(v.IsType<T>());
		REQUIRE(v.GetTypeID() == expectedId);
		REQUIRE(v.Get<T>() == value);

		T out{};
		v.GetVal(out);
		REQUIRE(out == value);

		const T* p = v.GetPtr<T>();
		REQUIRE(p != nullptr);
		REQUIRE(*p == value);

		// a wrong type is rejected, not reinterpreted
		if (expectedId != varType_CEmptyVariant)
		{
			REQUIRE_FALSE(v.IsType<CEmptyVariant>());
			REQUIRE_THROWS_AS(v.Get<CEmptyVariant>(), CExcBase);
			REQUIRE(v.GetPtr<CEmptyVariant>() == nullptr);
		}
	}
}

TEST_CASE("CVariant holds every simple type", "[variant][construct]")
{
	CheckHolds(true, varType_bool);
	CheckHolds(false, varType_bool);
	CheckHolds(int8_t(-100), varType_int8_t);
	CheckHolds(uint8_t(250), varType_uint8_t);
	CheckHolds(int16_t(-30000), varType_int16_t);
	CheckHolds(uint16_t(60000), varType_uint16_t);
	CheckHolds(int32_t(-2000000000), varType_int32_t);
	CheckHolds(uint32_t(4000000000u), varType_uint32_t);
	CheckHolds(std::numeric_limits<int64_t>::min(), varType_int64_t);
	CheckHolds(std::numeric_limits<uint64_t>::max(), varType_uint64_t);
	CheckHolds(3.25f, varType_float);
	CheckHolds(-1e300, varType_double);
}

TEST_CASE("CVariant holds strings", "[variant][construct][string]")
{
	CheckHolds(std::string("ansi text"), varType_astr_t);
	CheckHolds(std::string(), varType_astr_t);
	CheckHolds(std::string(1000, 'x'), varType_astr_t); // heap-allocated, beyond any SSO
	CheckHolds(std::wstring(L"wide text"), varType_wstr_t);
	CheckHolds(std::wstring(), varType_wstr_t);
}

TEST_CASE("CVariant holds a CGuid", "[variant][construct][guid]")
{
	const CGuid g = CGuid::CreateNew();
	CheckHolds(g, varType_CGuid);
	CheckHolds(CGuid::CreateNull(), varType_CGuid);

	CVariant v(g);
	REQUIRE(v.Get<CGuid>().ToAstr(true) == g.ToAstr(true));
}

TEST_CASE("CVariant holds a blob pointer and shares ownership", "[variant][construct][blob]")
{
	Data::CBlobPtr blob = MakeBlob({ 1, 2, 3 });
	REQUIRE(blob.use_count() == 1);

	{
		CVariant v(blob);
		REQUIRE(v.IsType<Data::CBlobPtr>());
		REQUIRE(v.GetTypeID() == varType_CBlobPtr_t);
		REQUIRE(blob.use_count() == 2);

		Data::CBlobPtr back = v.Get<Data::CBlobPtr>();
		REQUIRE(back == blob);
		REQUIRE(back->Size() == 3);
		REQUIRE((*back)[1] == 2);
	}

	// the variant's destructor released its reference
	REQUIRE(blob.use_count() == 1);
}

TEST_CASE("CVariant holds a (null) IGeoShapePtr", "[variant][construct]")
{
	IGeoShapePtr shape; // no concrete shape needed to exercise the slot
	CVariant v(shape);
	REQUIRE(v.IsType<IGeoShapePtr>());
	REQUIRE(v.GetTypeID() == varType_IGeoShapePtr);
	REQUIRE(v.Get<IGeoShapePtr>() == nullptr);
}

TEST_CASE("Plain int maps to int32_t", "[variant][construct]")
{
	int value = 42;
	CVariant v(value);
	REQUIRE(v.IsType<int32_t>());
	REQUIRE(v.Get<int>() == 42);
	REQUIRE(v.Get<int32_t>() == 42);
}

// ---------------------------------------------------------------------------
// Mutation through Get<T>() / SetVal / operator=
// ---------------------------------------------------------------------------

TEST_CASE("Get<T>() returns a reference into the variant", "[variant][mutate]")
{
	CVariant v(int32_t(1));
	v.Get<int32_t>() = 99;
	REQUIRE(v.Get<int32_t>() == 99);

	CVariant s(std::string("abc"));
	s.Get<std::string>() += "def";
	REQUIRE(s.Get<std::string>() == "abcdef");
	*s.GetPtr<std::string>() = "replaced";
	REQUIRE(s.Get<std::string>() == "replaced");
}

TEST_CASE("operator=(value) with the same type copies in place", "[variant][mutate]")
{
	CVariant v(int32_t(1));
	v = int32_t(2);
	REQUIRE(v.IsType<int32_t>());
	REQUIRE(v.Get<int32_t>() == 2);

	CVariant s(std::string("one"));
	s = std::string("two");
	REQUIRE(s.Get<std::string>() == "two");
}

TEST_CASE("operator=(value) with another type switches the type", "[variant][mutate]")
{
	CVariant v(int32_t(7));

	v = std::string("now a string");
	REQUIRE(v.IsType<std::string>());
	REQUIRE(v.Get<std::string>() == "now a string");
	REQUIRE_THROWS_AS(v.Get<int32_t>(), CExcBase);

	v = 2.5;
	REQUIRE(v.IsType<double>());
	REQUIRE(v.Get<double>() == 2.5);

	v = true;
	REQUIRE(v.IsType<bool>());
	REQUIRE(v.Get<bool>() == true);

	v = CEmptyVariant();
	REQUIRE(v.IsNull());
}

TEST_CASE("SetVal behaves like operator=(value)", "[variant][mutate]")
{
	CVariant v;
	v.SetVal(uint16_t(9));
	REQUIRE(v.IsType<uint16_t>());
	REQUIRE(v.Get<uint16_t>() == 9);

	v.SetVal(uint16_t(10));
	REQUIRE(v.Get<uint16_t>() == 10);

	v.SetVal(std::wstring(L"w"));
	REQUIRE(v.IsType<std::wstring>());
	REQUIRE(v.Get<std::wstring>() == L"w");
}

TEST_CASE("Switching away from a blob releases the reference", "[variant][mutate][blob]")
{
	Data::CBlobPtr blob = MakeBlob({ 9 });
	CVariant v(blob);
	REQUIRE(blob.use_count() == 2);

	v = int32_t(0);
	REQUIRE(blob.use_count() == 1);
	REQUIRE(v.IsType<int32_t>());

	v = blob;
	REQUIRE(blob.use_count() == 2);

	Data::CBlobPtr other = MakeBlob({ 8 });
	v = other; // same type: copy-assign the shared_ptr
	REQUIRE(blob.use_count() == 1);
	REQUIRE(other.use_count() == 2);
}

// ---------------------------------------------------------------------------
// Copy semantics
// ---------------------------------------------------------------------------

TEST_CASE("Copy constructor makes an independent copy", "[variant][copy]")
{
	SECTION("string")
	{
		CVariant a(std::string("original"));
		CVariant b(a);
		REQUIRE(b.IsType<std::string>());
		REQUIRE(b.Get<std::string>() == "original");

		b.Get<std::string>() = "changed";
		REQUIRE(a.Get<std::string>() == "original");
	}

	SECTION("scalar")
	{
		CVariant a(int64_t(-1));
		CVariant b(a);
		REQUIRE(b.Get<int64_t>() == -1);
		b = int64_t(5);
		REQUIRE(a.Get<int64_t>() == -1);
	}

	SECTION("empty")
	{
		CVariant a;
		CVariant b(a);
		REQUIRE(b.IsNull());
	}

	SECTION("blob shares the pointee")
	{
		Data::CBlobPtr blob = MakeBlob({ 1 });
		CVariant a(blob);
		CVariant b(a);
		REQUIRE(blob.use_count() == 3);
		REQUIRE(b.Get<Data::CBlobPtr>() == blob);
	}
}

TEST_CASE("Assignment from another variant", "[variant][copy]")
{
	SECTION("same type")
	{
		CVariant a(std::string("a"));
		CVariant b(std::string("b"));
		b = a;
		REQUIRE(b.Get<std::string>() == "a");
		a.Get<std::string>() = "a2";
		REQUIRE(b.Get<std::string>() == "a");
	}

	SECTION("different type replaces the payload")
	{
		Data::CBlobPtr blob = MakeBlob({ 1 });
		CVariant a(blob);
		CVariant b(std::string("text"));
		REQUIRE(blob.use_count() == 2);

		a = b;
		REQUIRE(a.IsType<std::string>());
		REQUIRE(a.Get<std::string>() == "text");
		REQUIRE(blob.use_count() == 1);

		b = CVariant();
		REQUIRE(b.IsNull());
		REQUIRE(a.Get<std::string>() == "text");
	}

	SECTION("self assignment is harmless")
	{
		CVariant a(std::string("self"));
		CVariant& alias = a;
		a = alias;
		REQUIRE(a.Get<std::string>() == "self");
	}

	SECTION("chained assignment")
	{
		CVariant a, b, c(double(1.5));
		a = b = c;
		REQUIRE(a.Get<double>() == 1.5);
		REQUIRE(b.Get<double>() == 1.5);
	}
}

TEST_CASE("Variants work inside standard containers", "[variant][copy]")
{
	std::vector<CVariant> values;
	values.push_back(CVariant(int32_t(3)));
	values.push_back(CVariant(std::string("s")));
	values.push_back(CVariant());
	values.push_back(CVariant(MakeBlob({ 1, 2 })));

	// force reallocation / copies
	for (int i = 0; i < 100; ++i)
		values.push_back(CVariant(double(i)));

	REQUIRE(values[0].Get<int32_t>() == 3);
	REQUIRE(values[1].Get<std::string>() == "s");
	REQUIRE(values[2].IsNull());
	REQUIRE(values[3].Get<Data::CBlobPtr>()->Size() == 2);
	REQUIRE(values[103].Get<double>() == 99.0);

	std::vector<CVariant> copy = values;
	REQUIRE(copy.size() == values.size());
	REQUIRE(copy[1].Get<std::string>() == "s");
	REQUIRE(copy[3].Get<Data::CBlobPtr>() == values[3].Get<Data::CBlobPtr>());
}

TEST_CASE("Destroying a variant runs the payload destructor", "[variant][lifetime]")
{
	std::weak_ptr<Data::CBlob> weak;
	{
		CVariant v(MakeBlob({ 1, 2, 3 }));
		weak = v.Get<Data::CBlobPtr>();
		REQUIRE_FALSE(weak.expired());
	}
	REQUIRE(weak.expired());
}
