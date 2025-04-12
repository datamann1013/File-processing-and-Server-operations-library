// Created by data0 on 11.04.2025.

#include "gtest/gtest.h"
#include "../FileCompression/CompressionAPI.h"
#include "../Errorhandler//ErrorCodes.h"

TEST(CompressionAPITest, CompressBlobValidInput) {
    std::string validJson = R"({
        "files": [
            {
                "fileName": "doc.txt",
                "fileType": "txt",
                "fileData": "Base64Data",
                "lastModified": "2023-10-10T15:30:00Z",
                "creationDate": "2023-09-01T12:00:00Z"
            }#include <gtest/gtest.h>
#include "../FileCompression/CompressionAPI.h"
#include "../errorhandler/ErrorCodes.h"

// Test that valid JSON input for compression returns SUCCESS and dummy data.
TEST(CompressionAPITest, CompressBlobValidInput) {
    std::string validJson = R"({
        "files": [
            {
                "fileName": "doc.txt",
                "fileType": "txt",
                "fileData": "Base64Data",
                "lastModified": "2023-10-10T15:30:00Z",
                "creationDate": "2023-09-01T12:00:00Z"
            }
        ],
        "options": {
            "compressionAlgorithm": "LZMA",
            "mode": "lossless",
            "preCompress": {"text": null},
            "encryption": false
        }
    })";

    auto result = CompressionAPI::compressBlob(validJson);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_EQ(result.compressedData, "DummyCompressedData");
}

// Test that an empty JSON input for compression returns an error.
TEST(CompressionAPITest, CompressBlobEmptyInput) {
    std::string emptyJson = "";
    auto result = CompressionAPI::compressBlob(emptyJson);
    // Expect user-side error (EU1) for invalid JSON.
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU1);
}
