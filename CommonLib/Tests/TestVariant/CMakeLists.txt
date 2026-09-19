set(BIN_NAME TestVariant)

set(SOURCE_FILES
        TestVariant.cpp
)

add_executable(${BIN_NAME} ${SOURCE_FILES})
target_link_libraries(${BIN_NAME} PUBLIC CommonLib)
IF (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE rpcrt4.lib)
ENDIF()