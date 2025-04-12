// Created by data0 on 11.04.2025.

#include <gtest/gtest.h>
#include "../FileCompression/CompressionAPI.h"
#include "../errorhandler/ErrorCodes.h"

// Test case: Valid JSON input should return SUCCESS and dummy compressed data.
TEST(CompressionAPITest, ValidCompressBlob) {
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

// Test case: Empty JSON input should return an error for invalid JSON.
TEST(CompressionAPITest, EmptyCompressBlob) {
    std::string emptyJson = "";
    auto result = CompressionAPI::compressBlob(emptyJson);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU1);
}
