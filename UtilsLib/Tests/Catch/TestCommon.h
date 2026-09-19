#pragma once

// Shared includes for the UtilsLib Catch2 tests.

#ifdef _WIN32
#define NOMINMAX
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "CommonLib/CommonLib.h"
#include "CommonLib/exception/exc_base.h"
#include "CommonLib/alloc/alloc.h"
#include "CommonLib/alloc/simpleAlloc.h"

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <set>
#include <algorithm>
