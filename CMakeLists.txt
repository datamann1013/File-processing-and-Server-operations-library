cmake_minimum_required(VERSION 3.30)
project(C__APIRepo)

set(CMAKE_CXX_STANDARD 20)

# Build main library
add_library(C__APIRepo SHARED library.cpp
        errorhandler/ErrorCodes.h
        errorhandler/ErrorHandler.cpp
        errorhandler/ErrorHandler.h
        FileCompression/CompressionAPI.cpp
        FileCompression/CompressionAPI.h)

# Executable testing library
add_executable(example library.cpp)
target_link_libraries(example PRIVATE cpprepoAPI)

add_subdirectory(Google_tests)
