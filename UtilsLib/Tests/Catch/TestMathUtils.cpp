#include "TestCommon.h"
#include "UtilsLib/MathUtils.h"

using namespace bptreedb::utils;

TEST_CASE("Log2 of powers of two is exact", "[mathutils][log2]")
{
	for (int i = 0; i < 60; ++i)
	{
		double v = (double)(uint64_t(1) << i);
		REQUIRE(Log2(v) == Catch::Approx((double)i).margin(1e-9));
	}
}

TEST_CASE("Log2 of 1 is 0", "[mathutils][log2]")
{
	REQUIRE(Log2(1.0) == Catch::Approx(0.0).margin(1e-12));
}

TEST_CASE("Log2 of non-powers of two", "[mathutils][log2]")
{
	REQUIRE(Log2(10.0) == Catch::Approx(3.321928094887362));
	REQUIRE(Log2(0.5) == Catch::Approx(-1.0));
	REQUIRE(Log2(3.0) == Catch::Approx(1.584962500721156));
}

TEST_CASE("Log2 matches std::log2", "[mathutils][log2]")
{
	const double values[] = { 0.001, 0.1, 1.5, 2.5, 7.0, 123.456, 1e6, 1e12 };
	for (double v : values)
		REQUIRE(Log2(v) == Catch::Approx(std::log2(v)).epsilon(1e-12));
}

TEST_CASE("Ln_2 constant", "[mathutils]")
{
	REQUIRE(Ln_2 == Catch::Approx(std::log(2.0)).epsilon(1e-15));
}
