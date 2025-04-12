project(Google_tests)

#Directory of Google test files
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Make test excecutable
add_executable(Google_Tests_run
        FileCompressionTest.cpp
        FileDeCompressionTest.cpp
)

target_link_libraries(Google_Tests_run gtest gtest_main)
target_link_libraries(Google_Tests_run FileCompression)

enable_testing()
add_test(NAME runTests COMMAND Google_Tests_run)
