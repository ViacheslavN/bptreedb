#pragma once

// Shared includes and helpers for the CommonLib/variant Catch2 tests.

#ifdef _WIN32
#define NOMINMAX
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "CommonLib/CommonLib.h"
#include "CommonLib/exception/exc_base.h"
#include "CommonLib/variant/Variant.h"
#include "CommonLib/variant/VariantVisitor.h"
#include "CommonLib/data/blob.h"
#include "CommonLib/guid/guid.h"

#include <string>
#include <vector>
#include <cstdint>
#include <limits>
#include <algorithm>

namespace test_utils
{
	inline CommonLib::Data::CBlobPtr MakeBlob(std::initializer_list<byte_t> bytes)
	{
		CommonLib::Data::CBlobPtr blob = std::make_shared<CommonLib::Data::CBlob>(0);
		for (byte_t b : bytes)
			blob->Push_back(b);
		return blob;
	}
}
