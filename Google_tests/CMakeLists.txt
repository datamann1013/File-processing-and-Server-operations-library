project(Google_tests)

#Directory of Google test files
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
enable_testing()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Make test excecutable
add_executable(CompressionDecompressionTests
        FileCompressionTest.cpp
        FileDeCompressionTest.cpp
        CompressionMethodTests.cpp
        Testassists/DummyfileGenerator/dummyFileGenerator.cpp
        Testassists/DummyfileGenerator/dummyFileGenerator.h
        Testassists/FileLocator/FileLocator.cpp
        Testassists/FileLocator/FileLocator.h
        AssistAlgorithmTests.cpp
        UnitTests/LZZ77UnitTests.cpp
        IntegrationTests/LZ77IntegrationTests.cpp
)
target_link_libraries(CompressionDecompressionTests gtest gtest_main FileCompression ErrorHandler)

add_test(NAME runTests COMMAND CompressionDecompressionTests)


add_executable(ErrorHandlerTests
        ErrorhandlerTests.cpp
)
target_include_directories(ErrorHandlerTests PRIVATE
        ${CMAKE_SOURCE_DIR}/Errorhandler
)
target_link_libraries(ErrorHandlerTests ErrorHandler gtest gtest_main)

add_test(NAME ErrorHandlerTests
        COMMAND ErrorHandlerTests)