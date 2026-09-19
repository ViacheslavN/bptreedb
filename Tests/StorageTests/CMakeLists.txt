set(BIN_NAME StorageTests)

set(SOURCE_FILES
        PageStreamTestcpp.cpp
        pch.cpp
        StorageTest.cpp
)



add_executable(${BIN_NAME} ${SOURCE_FILES})
target_link_libraries(${BIN_NAME} PUBLIC CommonLib)
target_link_libraries(${BIN_NAME} PUBLIC StorageLib)