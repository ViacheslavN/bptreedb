cmake_minimum_required(VERSION 3.17)
project(UtilsLib)


set(SOURCE_FILES
        CRC.cpp
        pch.cpp
        utils.cpp
)


add_library(UtilsLib ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} PUBLIC CommonLib)
target_include_directories(${PROJECT_NAME} PRIVATE ./)


option(UTILSLIB_BUILD_TESTS "Build UtilsLib Catch2 tests" ON)
if(UTILSLIB_BUILD_TESTS)
    add_subdirectory(Tests/Catch)
endif()
