// Created by data0 on 11.04.2025.

#include "gtest/gtest.h"
#include "../FileComrepssion/CompressionAPI.h"
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
    // We expect success when valid JSON is provided.
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_EQ(result.compressedData, "DummyCompressedData");
}

TEST(CompressionAPITest, CompressBlobEmptyInput) {
    std::string emptyJson = "";
    auto result = CompressionAPI::compressBlob(emptyJson);
    // We expect an INVALID_JSON error when an empty string is provided.
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::INVALID_JSON);
}


TEST(FileCompressionTestSuite, ExampleDate){ }