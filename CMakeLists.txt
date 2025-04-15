cmake_minimum_required(VERSION 3.30)
project(C__APIRepo)

set(CMAKE_CXX_STANDARD 20)

# Build main library
add_library(C__APIRepo SHARED library.cpp
        errorhandler/ErrorCodes.h
        FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.cpp
        FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.h
        FileCompression/CompressionMethods/AssistAlgorithms/Huffman/Huffman.cpp
        FileCompression/CompressionMethods/AssistAlgorithms/Huffman/Huffman.h
)

add_library(ErrorHandler SHARED library.cpp
        errorhandler/ErrorCodes.h
        errorhandler/ErrorHandler.cpp
        errorhandler/ErrorHandler.h
)

add_library(FileCompression SHARED library.cpp
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
        FileCompression/CompressionMethods/JPEG/JPEGCompressor.h
)

# Executable testing library
add_executable(example library.cpp)
target_link_libraries(example PRIVATE cpprepoAPI)
target_link_libraries(FileCompression PRIVATE ErrorHandler)

add_definitions(-DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/Google_tests/TestFiles")

add_subdirectory(Google_tests)




set_target_properties(ErrorHandler PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(FileCompression PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(CompressionDecompressionTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

