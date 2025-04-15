// Created by data0 on 11.04.2025.
#include <gtest/gtest.h>
#include "../FileCompression/CompressionAPI.h"
#include "../errorhandler/ErrorCodes.h"

#define ASSERT_NOT_IMPLEMENTED(result)                    \
if (result.errorCode == ErrorCodes::Compression::ENN99) { \
FAIL() << "Function not implemented (returned ENN99)";  \
}

// Test 1: Valid JSON for decompression should return SUCCESS.
TEST(DecompressionAPITest, ValidJSONReturnsSuccess) {
    std::string jsonInput = R"({
        "compressedData": "dummyCompressedData",
        "options": {"decompressionAlgorithm": "LZMA"}
    })";
    auto result = CompressionAPI::decompressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
}

// Test 2: Empty JSON returns error EU1.
TEST(DecompressionAPITest, EmptyJSONReturnsEU1) {
    std::string jsonInput = "";
    auto result = CompressionAPI::decompressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU1);
}

// Test 3: Simulated corrupted input returns error ES3.
TEST(DecompressionAPITest, CorruptedInputReturnsES3) {
    std::string jsonInput = R"({
        "compressedData": "corruptedData",
        "options": {"decompressionAlgorithm": "LZMA", "simulate_corrupted": true}
    })";
    auto result = CompressionAPI::decompressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::ES3);
}

// Test 4: Simulated data integrity error returns error ES4.
TEST(DecompressionAPITest, DataIntegrityErrorReturnsES4) {
    std::string jsonInput = R"({
        "compressedData": "dummyCompressedData",
        "options": {"decompressionAlgorithm": "LZMA", "simulate_integrity_error": true}
    })";
    auto result = CompressionAPI::decompressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::ES4);
}
