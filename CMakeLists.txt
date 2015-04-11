project(c-stacktrace)
cmake_minimum_required(VERSION 2.8)

set(SRC_FILES
        ./stacktrace.c
)
set(HEADER_FILES
        ./stacktrace.h
)
add_library(cstacktrace STATIC ${SRC_FILES} ${HEADER_FILES})

#build the test application also
add_subdirectory(testc ${CMAKE_CURRENT_BINARY_DIR}/testc)
#add_subdirectory(testcpp ${CMAKE_CURRENT_BINARY_DIR}/testcpp)
