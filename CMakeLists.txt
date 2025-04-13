cmake_minimum_required(VERSION 3.30)
project(C__APIRepo)

set(CMAKE_CXX_STANDARD 20)

# Build main library
add_library(C__APIRepo SHARED library.cpp
        errorhandler/ErrorCodes.h
        errorhandler/ErrorHandler.cpp
        errorhandler/ErrorHandler.h
        FileCompression/CompressionAPI.cpp
        FileCompression/CompressionAPI.h
        FileCompression/ICompressor.h
        FileCompression/CompressionMethods/LZMA/LZMACompressor.cpp
        FileCompression/CompressionMethods/LZMA/LZMACompressor.h
        FileCompression/CompressionMethods/ZIP/ZIPCompressor.cpp
        FileCompression/CompressionMethods/ZIP/ZIPCompressor.h
        FileCompression/CompressionMethods/H264/H264Compressor.cpp
        FileCompression/CompressionMethods/H264/H264Compressor.h
        FileCompression/CompressionMethods/JPEG/JPEGCompressor.cpp
        FileCompression/CompressionMethods/JPEG/JPEGCompressor.h)

# Executable testing library
add_executable(example library.cpp)
target_link_libraries(example PRIVATE cpprepoAPI)

add_subdirectory(Google_tests)
