set(BIN_NAME TestSQlite)

set(SOURCE_FILES
        TestSQlite.cpp
)



add_executable(${BIN_NAME} ${SOURCE_FILES})
target_link_libraries(${BIN_NAME} PUBLIC CommonLib)