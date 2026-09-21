cmake_minimum_required(VERSION 3.17)

# Catch2 unit tests for CommonLib/stream.
# Catch2 v3 comes from vcpkg:  vcpkg install catch2   (x64-windows)
set(BIN_NAME CommonLibStreamCatchTests)

find_package(Catch2 3 CONFIG REQUIRED)

set(SOURCE_FILES
        TestCommon.h
        TestMemoryStreamBuffer.cpp
        TestMemoryStream.cpp
        TestFixedMemoryStream.cpp
        TestBitStream.cpp
        TestStreamIO.cpp
        TestFileStream.cpp
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
