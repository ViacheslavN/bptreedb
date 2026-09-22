cmake_minimum_required(VERSION 3.17)
project(SpatialLib)


set(SOURCE_FILES
    ZCurve/Point/SpatialPointQuery.cpp
    ZCurve/Rect/SpatialRectQuery.cpp
)


add_library(SpatialLib ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} PUBLIC CommonLib)

target_include_directories(${PROJECT_NAME} PUBLIC ./)

option(SPATIALLIB_BUILD_TESTS "Build SpatialLib Catch2 tests" ON)
if(SPATIALLIB_BUILD_TESTS)
    add_subdirectory(Tests/Catch)
    add_subdirectory(Tests/TestRectIndex)
endif()

