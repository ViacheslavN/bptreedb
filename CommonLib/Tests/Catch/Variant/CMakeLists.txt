cmake_minimum_required(VERSION 3.17)

# Catch2 unit tests for CommonLib/variant.
# Catch2 v3 comes from vcpkg:  vcpkg install catch2   (x64-windows)
set(BIN_NAME CommonLibVariantCatchTests)

find_package(Catch2 3 CONFIG REQUIRED)

set(SOURCE_FILES
        TestCommon.h
        TestVariant.cpp
        TestVariantCompare.cpp
        TestVariantVisitor.cpp
)

add_executable(${BIN_NAME} ${SOURCE_FILES})

# Headers are included as "CommonLib/xxx.h" relative to the repo root
target_include_directories(${BIN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)

target_link_libraries(${BIN_NAME} PRIVATE
        CommonLib
        Catch2::Catch2WithMain
)

# Register every TEST_CASE with CTest
include(CTest)
include(Catch)
catch_discover_tests(${BIN_NAME})
