// Created by data0 on 11.04.2025.

#include <gtest/gtest.h>
#include "../FileCompression/CompressionAPI.h"
#include "../errorhandler/ErrorCodes.h"

// Test that valid JSON input for decompression returns SUCCESS and dummy data.
TEST(DeCompressionAPITest, DecompressBlobValidInput) {
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

// Test that an empty JSON input for decompression returns an error.
TEST(DeCompressionAPITest, DecompressBlobEmptyInput) {
    std::string emptyJson = "";
    auto result = CompressionAPI::decompressBlob(emptyJson);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU1);
}
