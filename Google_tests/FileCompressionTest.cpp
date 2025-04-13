// Created by data0 on 11.04.2025.

#include <gtest/gtest.h>
#include "../FileCompression/CompressionAPI.h"
#include "../errorhandler/ErrorCodes.h"
#include "dummyFileGenerator.h"
#include <vector>

// Test 1: Valid JSON Parsing
TEST(CompressionAPITest, ValidJSONParsing) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.txt", "fileType": "txt", "fileData": "dummyData",
             "lastModified": "2023-10-10T15:30:00Z", "creationDate": "2023-09-01T12:00:00Z"}
        ],
        "options": {
            "compressionAlgorithm": "LZMA",
            "mode": "lossless",
            "preCompress": {"text": null},
            "encryption": false
        }
    })";
    auto result = CompressionAPI::compressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
}

// Test 2: Empty JSON returns error EU1.
TEST(CompressionAPITest, EmptyJSONReturnsEU1) {
    std::string jsonInput = "";
    auto result = CompressionAPI::compressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU1);
}

// Test 3: Unsupported file type returns error EU2.
TEST(CompressionAPITest, UnsupportedFileTypeReturnsEU2) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.xyz", "fileType": "unsupportedFileType", "fileData": "dummyData",
             "lastModified": "2023-10-10T15:30:00Z", "creationDate": "2023-09-01T12:00:00Z"}
        ],
        "options": {
            "compressionAlgorithm": "LZMA",
            "mode": "lossless",
            "preCompress": {"text": null},
            "encryption": false
        }
    })";
    auto result = CompressionAPI::compressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU2);
}

// Test 4: Invalid precompression parameters trigger warning WU1.
TEST(CompressionAPITest, InvalidPrecompressionParameters) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.txt", "fileType": "txt", "fileData": "dummyData",
             "lastModified": "2023-10-10T15:30:00Z", "creationDate": "2023-09-01T12:00:00Z"}
        ],
        "options": {
            "compressionAlgorithm": "LZMA",
            "mode": "lossless",
            "preCompress": {"simulate_invalid_precompress": true},
            "encryption": false
        }
    })";
    auto result = CompressionAPI::compressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::WU1);
}

// Test 5: Compression Ratio Test
// Sizes: 1MB, 50MB, 100MB, 1GB, 5GB, 15GB, run 5 iterations each, compute average ratio
TEST(CompressionAPITest, CompressionRatioTest) {
    struct TestCase { size_t fileSize; double maxRatio; };
    std::vector<TestCase> testCases = {
        {1048576, 0.2},       // 1MB, compressed < 20%
        {52428800, 0.25},     // 50MB
        {104857600, 0.25},    // 100MB
        {1073741824, 0.3},     // 1GB
        {5368709120, 0.35},    // 5GB
        {16106127360, 0.4}     // 15GB
    };

    for (const auto& test : testCases) {
        // Generate dummy file data once.
        std::string fileData = generateDummyFileData(test.fileSize);

        double totalRatio = 0.0;
        const int iterations = 5;
        for (int i = 0; i < iterations; ++i) {
            // Build JSON input.
            std::string jsonInput = "{\"files\": ["
                                    "{\"fileName\": \"dummy.txt\", "
                                    "\"fileType\": \"txt\", "
                                    "\"fileData\": \"" + fileData +
                                    "\", \"lastModified\": \"2023-10-10T15:30:00Z\", "
                                    "\"creationDate\": \"2023-09-01T12:00:00Z\"}"
                                    "], \"options\": {"
                                    "\"compressionAlgorithm\": \"LZMA\", "
                                    "\"mode\": \"lossless\", "
                                    "\"preCompress\": {\"text\": null}, "
                                    "\"encryption\": false}}";

            auto result = CompressionAPI::compressBlob(jsonInput);
            EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
            size_t compressedSize = result.data.size();
            double ratio = static_cast<double>(compressedSize) / test.fileSize;
            totalRatio += ratio;
        }
        double averageRatio = totalRatio / iterations;
        EXPECT_LT(averageRatio, test.maxRatio);
    }
}

// Test 6: Checksum Mismatch - Simulate checksum mismatch should return error ES2.
TEST(CompressionAPITest, ChecksumMismatchReturnsES2) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.txt", "fileType": "txt", "fileData": "dummyData",
             "lastModified": "2023-10-10T15:30:00Z", "creationDate": "2023-09-01T12:00:00Z"}
        ],
        "options": {
            "compressionAlgorithm": "LZMA",
            "mode": "lossless",
            "preCompress": {"text": null},
            "encryption": false,
            "simulate_checksum_mismatch": true
        }
    })";
    auto result = CompressionAPI::compressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::ES2);
}

// Test 7: Encryption Flag - When encryption is requested, return warning WU2.
TEST(CompressionAPITest, EncryptionFlagReturnsWU2) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.txt", "fileType": "txt", "fileData": "dummyData",
             "lastModified": "2023-10-10T15:30:00Z", "creationDate": "2023-09-01T12:00:00Z"}
        ],
        "options": {
            "compressionAlgorithm": "LZMA",
            "mode": "lossless",
            "preCompress": {"text": null},
            "encryption": true
        }
    })";
    auto result = CompressionAPI::compressBlob(jsonInput);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::WU2);
}
