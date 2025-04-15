project(Google_tests)

#Directory of Google test files
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Make test excecutable
add_executable(CompressionDecompressionTests
        FileCompressionTest.cpp
        FileDeCompressionTest.cpp
        CompressionMethodTests.cpp
        TestFiles/Testassists/DummyfileGenerator/dummyFileGenerator.cpp
        TestFiles/Testassists/DummyfileGenerator/dummyFileGenerator.h
        TestFiles/Testassists/FileLocator/FileLocator.cpp
        TestFiles/Testassists/FileLocator/FileLocator.h
)

target_link_libraries(CompressionDecompressionTests gtest gtest_main)
target_link_libraries(CompressionDecompressionTests FileCompression)
target_link_libraries(CompressionDecompressionTests ErrorHandler)

enable_testing()
add_test(NAME runTests COMMAND CompressionDecompressionTests)
