cmake_minimum_required(VERSION 3.17)
project(bptreedb)

find_library(PTHREAD_LIBRARY pthread)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        stdafx.cpp

		BPTrees/BPBaseTreeNode.cpp
		BPTrees/Compressor/EntropyEncoder/BaseNumLenEncoder.cpp
		BPTrees/Compressor/CompressorParams.cpp

		storage/FilePage.cpp
		storage/FileStorage.cpp

		utils/CRC.cpp
		)
set(STATIC_LIBS
        -lpthread
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -fPIC -fvisibility=hidden")

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${STATIC_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ./)
