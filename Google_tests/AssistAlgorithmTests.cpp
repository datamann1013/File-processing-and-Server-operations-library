// Created by admin on 15.04.2025.

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.h"
#include "../FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.cpp"

namespace fs = std::filesystem;



TEST(AssistAlgorithmTests, FileLocatorFilesDirectory) {
    const fs::path path(TEST_FILES_DIR);
    std::cout << "Current working directory: " << fs::current_path() << std::endl;
    EXPECT_TRUE(fs::exists(path)) << "TestFiles directory not found: " << path.string();
}


TEST(AssistAlgorithmTests, LZ77CompressTest) {
    // Open the test file
    std::ifstream file(TEST_FILE_SINGULAR);
    ASSERT_TRUE(file.is_open()) << "Could not open file: TEST_FILE_SINGULAR";

    // Read the file's content into a string.
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string inputData = buffer.str();

    auto result = CompressionAPI::compressBlob(inputData, true, true);


    // - Check if decompression reverses the compression.
    // - Validate output sizes or tokens.
}