project(Google_tests)

#Directory of Google test files
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Make test excecutable
add_executable(CompressionDecompressionTests
        FileCompressionTest.cpp
        FileDeCompressionTest.cpp
        CompressionMethodTests.cpp
        dummyFileGenerator.cpp
        dummyFileGenerator.h
)

target_link_libraries(CompressionDecompressionTests gtest gtest_main)
target_link_libraries(CompressionDecompressionTests FileCompression)
target_link_libraries(CompressionDecompressionTests ErrorHandler)

enable_testing()
add_test(NAME runTests COMMAND CompressionDecompressionTests)
