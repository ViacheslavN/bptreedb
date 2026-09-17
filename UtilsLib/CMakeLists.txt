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

