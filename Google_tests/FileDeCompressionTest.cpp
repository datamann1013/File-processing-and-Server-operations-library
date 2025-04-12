// Created by data0 on 11.04.2025.

#include <gtest/gtest.h>
#include "../FileCompression/CompressionAPI.h"
#include "../errorhandler/ErrorCodes.h"

// Test case: Valid JSON input for decompression returns SUCCESS and dummy decompressed data.
TEST(DecompressionAPITest, ValidDecompressBlob) {
    std::string validJson = R"({
        "compressedData": "SomeCompressedData",
        "options": {
            "decompressionAlgorithm": "LZMA"
        }
    })";

    auto result = CompressionAPI::decompressBlob(validJson);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_EQ(result.compressedData, "DummyDecompressedData");
}

// Test case: Empty JSON input for decompression should return invalid JSON error.
TEST(DecompressionAPITest, EmptyDecompressBlob) {
    std::string emptyJson = "";
    auto result = CompressionAPI::decompressBlob(emptyJson);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU1);
}
