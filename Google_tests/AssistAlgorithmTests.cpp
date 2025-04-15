// Created by admin on 15.04.2025.

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "../FileCompression/CompressionMethods/AssistAlgorithms/LZ77/LZ77.h"

#define ASSERT_NOT_IMPLEMENTED(result)                                                        \
if (result.errorCode == ErrorCodes::Compression::ENN99) {                                   \
FAIL() << "Function not implemented (returned ENN99)";                                \
}

TEST(LZ77Test, CompressTest) {
    // Open the test file from the provided content root.
    std::ifstream file(TEST_FILE_SINGULAR);
    ASSERT_TRUE(file.is_open()) << "Could not open file: TEST_FILE_SINGULAR";

    // Read the file's content into a string.
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string inputData = buffer.str();

    // Call the compress function.
    auto result = CompressionAPI::compressBlob(inputData);

    // Assert that the function returns ENN99 (not implemented).
    ASSERT_NOT_IMPLEMENTED(result);

    // Further validations can be added here after implementation, for example:
    // - Check if decompression reverses the compression.
    // - Validate output sizes or tokens.
}
