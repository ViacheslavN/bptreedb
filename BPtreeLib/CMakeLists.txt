cmake_minimum_required(VERSION 3.17)
project(BPtreeLib)


set(SOURCE_FILES_COMPRESSOR
        Compressor/EntropyEncoder/BaseNumLenEncoder.cpp
        Compressor/EntropyEncoder/BitStream.cpp
        Compressor/EntropyEncoder/BoolEncoder.cpp
        Compressor/EntropyEncoder/SignedNumLenEncoder.cpp
        Compressor/EntropyEncoder/UnsignedNumLenEncoder.cpp

        Compressor/StringCompressor/EmptyStringEncoder.cpp
        Compressor/StringCompressor/ZLibStringEncoder.cpp

        Compressor/CompressorParams.cpp
)

set(SOURCE_FILES_BPTREE
        AllocsSet.cpp
        BPBaseTreeNode.cpp
        BPTreeContext.cpp
        StringPageAlloc.cpp
        StringVal.cpp

)

set(SOURCE_FILES ${SOURCE_FILES_COMPRESSOR} ${SOURCE_FILES_BPTREE})

add_library(BPtreeLib ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} PUBLIC CommonLib)
target_link_libraries(${PROJECT_NAME} PUBLIC UtilsLib)
target_link_libraries(${PROJECT_NAME} PUBLIC StorageLib)
target_include_directories(${PROJECT_NAME} PRIVATE ./)

