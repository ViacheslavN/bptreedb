set(BIN_NAME TestStr)

set(SOURCE_FILES
        TestConvert.cpp
        TestStr.cpp
)

add_executable(${BIN_NAME} ${SOURCE_FILES})
target_link_libraries(${BIN_NAME} PUBLIC CommonLib)