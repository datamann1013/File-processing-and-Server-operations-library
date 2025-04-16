// Created by data0 on 11.04.2025.
#include <gtest/gtest.h>
#include "../FileCompression/CompressionAPI.h"
#include "../errorhandler/ErrorCodes.h"

#define ASSERT_NOT_IMPLEMENTED(result)                    \
if (result.errorCode == ErrorCodes::Compression::ENN99) { \
FAIL() << "Function not implemented (returned ENN99)";  \
}

TEST(DecompressionAPITest, ValidJSONReturnsSuccess) {
    std::string jsonInput = R"({
        "compressedData": "dummyCompressedData",
        "options": {"decompressionAlgorithm": "LZMA"}
    })";
    auto result = CompressionAPI::decompressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
}

TEST(DecompressionAPITest, EmptyJSONReturnsEU1) {
    std::string jsonInput = "";
    auto result = CompressionAPI::decompressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU1);
}

TEST(DecompressionAPITest, CorruptedInputReturnsES3) {
    std::string jsonInput = R"({
        "compressedData": "corruptedData",
        "options": {"decompressionAlgorithm": "LZMA", "simulate_corrupted": true}
    })";
    auto result = CompressionAPI::decompressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::ES3);
}

TEST(DecompressionAPITest, DataIntegrityErrorReturnsES4) {
    std::string jsonInput = R"({
        "compressedData": "dummyCompressedData",
        "options": {"decompressionAlgorithm": "LZMA", "simulate_integrity_error": true}
    })";
    auto result = CompressionAPI::decompressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::ES4);
}
