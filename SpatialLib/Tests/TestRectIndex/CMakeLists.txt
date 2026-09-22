set(BIN_NAME TestRectIndex)

set(SOURCE_FILES
        main.cpp
)
add_executable(${BIN_NAME} ${SOURCE_FILES})

# SpatialLib's headers include "SpatialKey.h" and "pch.h" relative to the SpatialLib
# root; the repo root lets headers be included as "SpatialLib/..." / "CommonLib/...".
target_include_directories(${BIN_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../..
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(${BIN_NAME} PUBLIC CommonLib)
target_link_libraries(${BIN_NAME} PUBLIC SpatialLib)