#include "TestCommon.h"

using namespace CommonLib;
using test_utils::MakeBlob;

namespace
{
	// Records which overload was dispatched and what it received.
	class CRecordingVisitor : public IVisitor
	{
	public:
		eDataTypes hit = varType_CEmptyVariant;
		int calls = 0;

		bool vBool = false;
		int64_t vInt = 0;
		uint64_t vUInt = 0;
		double vReal = 0;
		std::string vStr;
		std::wstring vWStr;
		Data::CBlobPtr vBlob;
		IGeoShapePtr vShape;
		CGuid vGuid;

		void Visit(const CEmptyVariant&) override { Mark(varType_CEmptyVariant); }
		void Visit(const bool& v) override { Mark(varType_bool); vBool = v; }
		void Visit(const int8_t& v) override { Mark(varType_int8_t); vInt = v; }
		void Visit(const uint8_t& v) override { Mark(varType_uint8_t); vUInt = v; }
		void Visit(const int16_t& v) override { Mark(varType_int16_t); vInt = v; }
		void Visit(const uint16_t& v) override { Mark(varType_uint16_t); vUInt = v; }
		void Visit(const int32_t& v) override { Mark(varType_int32_t); vInt = v; }
		void Visit(const uint32_t& v) override { Mark(varType_uint32_t); vUInt = v; }
		void Visit(const int64_t& v) override { Mark(varType_int64_t); vInt = v; }
		void Visit(const uint64_t& v) override { Mark(varType_uint64_t); vUInt = v; }
		void Visit(const float& v) override { Mark(varType_float); vReal = v; }
		void Visit(const double& v) override { Mark(varType_double); vReal = v; }
		void Visit(const std::string& v) override { Mark(varType_astr_t); vStr = v; }
		void Visit(const std::wstring& v) override { Mark(varType_wstr_t); vWStr = v; }
		void Visit(const Data::CBlobPtr& v) override { Mark(varType_CBlobPtr_t); vBlob = v; }
		void Visit(const IGeoShapePtr& v) override { Mark(varType_IGeoShapePtr); vShape = v; }
		void Visit(const CGuid& v) override { Mark(varType_CGuid); vGuid = v; }

	private:
		void Mark(eDataTypes id) { hit = id; ++calls; }
	};
}

TEST_CASE("Accept dispatches to the overload matching the stored type", "[variant][visitor]")
{
	CRecordingVisitor rec;

	SECTION("empty")
	{
		CVariant().Accept(rec);
		REQUIRE(rec.hit == varType_CEmptyVariant);
	}
	SECTION("bool")
	{
		CVariant(true).Accept(rec);
		REQUIRE(rec.hit == varType_bool);
		REQUIRE(rec.vBool == true);
	}
	SECTION("signed integers")
	{
		CVariant(int8_t(-8)).Accept(rec);   REQUIRE(rec.hit == varType_int8_t);  REQUIRE(rec.vInt == -8);
		CVariant(int16_t(-16)).Accept(rec); REQUIRE(rec.hit == varType_int16_t); REQUIRE(rec.vInt == -16);
		CVariant(int32_t(-32)).Accept(rec); REQUIRE(rec.hit == varType_int32_t); REQUIRE(rec.vInt == -32);
		CVariant(int64_t(-64)).Accept(rec); REQUIRE(rec.hit == varType_int64_t); REQUIRE(rec.vInt == -64);
	}
	SECTION("unsigned integers")
	{
		CVariant(uint8_t(8)).Accept(rec);   REQUIRE(rec.hit == varType_uint8_t);  REQUIRE(rec.vUInt == 8);
		CVariant(uint16_t(16)).Accept(rec); REQUIRE(rec.hit == varType_uint16_t); REQUIRE(rec.vUInt == 16);
		CVariant(uint32_t(32)).Accept(rec); REQUIRE(rec.hit == varType_uint32_t); REQUIRE(rec.vUInt == 32);
		CVariant(uint64_t(64)).Accept(rec); REQUIRE(rec.hit == varType_uint64_t); REQUIRE(rec.vUInt == 64);
	}
	SECTION("floating point")
	{
		CVariant(1.5f).Accept(rec); REQUIRE(rec.hit == varType_float);  REQUIRE(rec.vReal == 1.5);
		CVariant(2.5).Accept(rec);  REQUIRE(rec.hit == varType_double); REQUIRE(rec.vReal == 2.5);
	}
	SECTION("strings")
	{
		CVariant(std::string("s")).Accept(rec);   REQUIRE(rec.hit == varType_astr_t); REQUIRE(rec.vStr == "s");
		CVariant(std::wstring(L"w")).Accept(rec); REQUIRE(rec.hit == varType_wstr_t); REQUIRE(rec.vWStr == L"w");
	}
	SECTION("guid")
	{
		const CGuid g = CGuid::CreateNew();
		CVariant(g).Accept(rec);
		REQUIRE(rec.hit == varType_CGuid);
		REQUIRE(rec.vGuid == g);
	}
	SECTION("blob and shape pointers")
	{
		Data::CBlobPtr blob = MakeBlob({ 7 });
		CVariant(blob).Accept(rec);
		REQUIRE(rec.hit == varType_CBlobPtr_t);
		REQUIRE(rec.vBlob == blob);

		CVariant(IGeoShapePtr()).Accept(rec);
		REQUIRE(rec.hit == varType_IGeoShapePtr);
		REQUIRE(rec.vShape == nullptr);
	}

	REQUIRE(rec.calls >= 1);
}

TEST_CASE("Accept follows type changes of the variant", "[variant][visitor]")
{
	CRecordingVisitor rec;
	CVariant v(int32_t(1));
	v.Accept(rec);
	REQUIRE(rec.hit == varType_int32_t);

	v = std::string("now text");
	v.Accept(rec);
	REQUIRE(rec.hit == varType_astr_t);
	REQUIRE(rec.vStr == "now text");

	v = CVariant();
	v.Accept(rec);
	REQUIRE(rec.hit == varType_CEmptyVariant);
	REQUIRE(rec.calls == 3);
}

TEST_CASE("ApplyVisitor visits once and returns the visitor by value", "[variant][visitor]")
{
	CRecordingVisitor rec;
	CRecordingVisitor result = ApplyVisitor(CVariant(uint32_t(77)), rec);

	REQUIRE(rec.calls == 1);
	REQUIRE(rec.hit == varType_uint32_t);
	REQUIRE(result.calls == 1);
	REQUIRE(result.vUInt == 77);
}

TEST_CASE("CStringVisitor renders values as text", "[variant][visitor][string]")
{
	CStringVisitor sv;

	SECTION("integers")
	{
		CVariant(int32_t(42)).Accept(sv);
		REQUIRE(sv.GetString() == "42");
		CVariant(int64_t(-5)).Accept(sv);
		REQUIRE(sv.GetString() == "-5");
		CVariant(uint64_t(18446744073709551615ULL)).Accept(sv);
		REQUIRE(sv.GetString() == "18446744073709551615");
		CVariant(uint8_t(255)).Accept(sv);
		REQUIRE(sv.GetString() == "255");
		CVariant(int8_t(-128)).Accept(sv);
		REQUIRE(sv.GetString() == "-128");
	}

	SECTION("bool renders as 1 / 0")
	{
		CVariant(true).Accept(sv);
		REQUIRE(sv.GetString() == "1");
		CVariant(false).Accept(sv);
		REQUIRE(sv.GetString() == "0");
	}

	SECTION("strings pass through")
	{
		CVariant(std::string("plain")).Accept(sv);
		REQUIRE(sv.GetString() == "plain");
		CVariant(std::wstring(L"wide")).Accept(sv);
		REQUIRE(sv.GetString() == "wide");
	}

	SECTION("guid renders with brackets")
	{
		const CGuid g = CGuid::CreateNew();
		CVariant(g).Accept(sv);
		REQUIRE(sv.GetString() == g.ToAstr(true));
	}

	SECTION("floating point is non-empty")
	{
		CVariant(1.5).Accept(sv);
		REQUIRE_FALSE(sv.GetString().empty());
		CVariant(2.5f).Accept(sv);
		REQUIRE_FALSE(sv.GetString().empty());
	}

	SECTION("empty, blob and shape render as an empty string")
	{
		CVariant(std::string("reset me")).Accept(sv);
		CVariant().Accept(sv);
		REQUIRE(sv.GetString().empty());

		CVariant(std::string("reset me")).Accept(sv);
		CVariant(MakeBlob({ 1 })).Accept(sv);
		REQUIRE(sv.GetString().empty());

		CVariant(std::string("reset me")).Accept(sv);
		CVariant(IGeoShapePtr()).Accept(sv);
		REQUIRE(sv.GetString().empty());
	}
}
