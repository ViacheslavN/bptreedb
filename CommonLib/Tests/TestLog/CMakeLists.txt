set(BIN_NAME TestLog)

find_library(PTHREAD_LIBRARY pthread)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -fPIC -fvisibility=hidden  -pthread")

set(STATIC_LIBS
        -lpthread
        )


set(SOURCE_FILES
        TestLog.cpp
        TestZlib.cpp
)

IF (!WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
ENDIF()

add_executable(${BIN_NAME} ${SOURCE_FILES})
target_link_libraries(${BIN_NAME} PRIVATE ${STATIC_LIBS})
target_link_libraries(${BIN_NAME} PRIVATE CommonLib)
#target_include_directories(${BIN_NAME} PRIVATE ../../../)

 
