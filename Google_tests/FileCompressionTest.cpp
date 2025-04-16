// Created by data0 on 11.04.2025. Compression tests

#include <gtest/gtest.h>
#include "../FileCompression/CompressionAPI.h"
#include "../errorhandler/ErrorCodes.h"
#include "Testassists/FileLocator/FileLocator.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#define ASSERT_NOT_IMPLEMENTED(result)                    \
    if (result.errorCode == ErrorCodes::Compression::ENN99) { \
        FAIL() << "Function not implemented (returned ENN99)";  \
    }

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
    ASSERT_NOT_IMPLEMENTED(result);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
}

TEST(CompressionAPITest, EmptyJSONReturnsEU1) {
    std::string jsonInput = "";
    auto result = CompressionAPI::compressBlob(jsonInput);
    ASSERT_NOT_IMPLEMENTED(result);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU1);
}

TEST(CompressionAPITest, SpecificAlgorithmSelection) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.txt", "fileType": "txt", "fileData": "data",
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
    ASSERT_NOT_IMPLEMENTED(result);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
}

TEST(CompressionAPITest, NoAlgorithmSpecifiedDefaultsToLZMA) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.txt", "fileType": "txt", "fileData": "data",
             "lastModified": "2023-10-10T15:30:00Z", "creationDate": "2023-09-01T12:00:00Z"}
        ],
        "options": {
            "mode": "lossless",
            "preCompress": {"text": null},
            "encryption": false
        }
    })";
    auto result = CompressionAPI::compressBlob(jsonInput);
    ASSERT_NOT_IMPLEMENTED(result);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS);
}

TEST(CompressionAPITest, UnsupportedFileTypeReturnsEU2) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.xyz", "fileType": "unsupportedFileType", "fileData": "data",
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
    ASSERT_NOT_IMPLEMENTED(result);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::EU2);
}

TEST(CompressionAPITest, InvalidPrecompressionParameters) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.txt", "fileType": "txt", "fileData": "data",
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
    ASSERT_NOT_IMPLEMENTED(result);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::WU1);
}

TEST(CompressionAPITest, MissingMetadataTriggersWarning) {
    std::string jsonInput = R"({
        "files": [
            {"fileName": "doc.txt", "fileType": "txt", "fileData": "data"}
        ],
        "options": {
            "compressionAlgorithm": "LZMA",
            "mode": "lossless",
            "preCompress": {"text": null},
            "encryption": false,
            "simulate_missing_metadata": true
        }
    })";
    auto result = CompressionAPI::compressBlob(jsonInput);
    ASSERT_NOT_IMPLEMENTED(result);
    EXPECT_EQ(result.errorCode, ErrorCodes::Compression::WU1);
}

// For each file in TestFiles run 3 iterations per file and verify that average is below an expected threshold.
TEST(CompressionAPITest, CompressionRatioTest) {
    std::string testFilesDir = TEST_FILES_DIR;
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    auto files = FileCompression::FileLocator::getFilesByType(testFilesDir, "null");
    ASSERT_FALSE(files.empty()) << "No test files found in TestFiles directory.";

    const int iterations = 3;
    const double maxRatio = 0.25; // Expected maximum ratio.

    for (const auto &filePath : files) {
        std::ifstream inFile(filePath, std::ios::binary);
        std::ostringstream oss;
        oss << inFile.rdbuf();
        std::string fileData = oss.str();
        size_t originalSize = fileData.size();
        if (originalSize == 0) continue;

        double totalRatio = 0.0;
        for (int i = 0; i < iterations; ++i) {
            // Construct JSON input with file data.
            std::string jsonInput = "{\"files\": ["
                                    "{\"fileName\": \"" + filePath.filename().string() + "\", "
                                    "\"fileType\": \"" + filePath.extension().string() + "\", "
                                    "\"fileData\": \"" + fileData +
                                    "\", \"lastModified\": \"2023-10-10T15:30:00Z\", "
                                    "\"creationDate\": \"2023-09-01T12:00:00Z\"}"
                                    "], \"options\": {"
                                    "\"compressionAlgorithm\": \"LZMA\", "
                                    "\"mode\": \"lossless\", "
                                    "\"preCompress\": {\"text\": null}, "
                                    "\"encryption\": false}}";

            auto result = CompressionAPI::compressBlob(jsonInput);
            ASSERT_NOT_IMPLEMENTED(result);
            EXPECT_EQ(result.errorCode, ErrorCodes::Compression::SUCCESS)
                << "Compression failed for file: " << filePath;
            size_t compressedSize = result.data.size();
            double ratio = static_cast<double>(compressedSize) / originalSize;
            totalRatio += ratio;
        }
        double averageRatio = totalRatio / iterations;
        EXPECT_LT(averageRatio, maxRatio)
            << "Average compression ratio for file " << filePath.filename().string()
            << " is " << averageRatio << ", which exceeds the threshold of " << maxRatio;
    }
}
