cmake_minimum_required(VERSION 3.17)
project(StorageLib)


set(SOURCE_FILES
     FilePage.cpp
     FileStorage.cpp
     PageMemoryBuffer.cpp
     ReadStreamPage.cpp
     storage.cpp
     WriteStreamPage.cpp
)


add_library(StorageLib ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} PUBLIC CommonLib)
target_link_libraries(${PROJECT_NAME} PUBLIC UtilsLib)
target_include_directories(${PROJECT_NAME} PRIVATE ./)

