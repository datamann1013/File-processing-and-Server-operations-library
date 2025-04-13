// Created by data0 on 13.04.2025.

#include <gtest/gtest.h>
#include "../FileCompression/CompressionMethods/LZMA/LZMACompressor.h"
#include "../FileCompression/CompressionMethods/ZIP/ZIPCompressor.h"
#include "../FileCompression/CompressionMethods/H264/H264Compressor.h"
#include "../FileCompression/CompressionMethods/JPEG/JPEGCompressor.h"
#include "../errorhandler/ErrorCodes.h"

// Helper macro to check if a method is not implemented.
#define ASSERT_NOT_IMPLEMENTED(result) \
    if (result.errorCode == ErrorCodes::Compression::ENN99) { \
        FAIL() << "Function not implemented (returned ENN99)"; \
    }

// Test LZMACompressor compress and decompress.
TEST(LZMACompressorTest, CompressAndDecompress) {
    LZMACompressor compressor;
    auto compResult = compressor.compress("TestData");
    ASSERT_NOT_IMPLEMENTED(compResult);
    EXPECT_NE(compResult.data, ""); // Expect some non-empty compressed data.

    auto decompResult = compressor.decompress(compResult.data);
    ASSERT_NOT_IMPLEMENTED(decompResult);
    EXPECT_NE(decompResult.data, "");
}

// Test ZIPCompressor compress and decompress.
TEST(ZIPCompressorTest, CompressAndDecompress) {
    ZIPCompressor compressor;
    auto compResult = compressor.compress("TestData");
    ASSERT_NOT_IMPLEMENTED(compResult);
    EXPECT_NE(compResult.data, "");

    auto decompResult = compressor.decompress(compResult.data);
    ASSERT_NOT_IMPLEMENTED(decompResult);
    EXPECT_NE(decompResult.data, "");
}

// Test H264Compressor compress and decompress.
TEST(H264CompressorTest, CompressAndDecompress) {
    H264Compressor compressor;
    auto compResult = compressor.compress("TestVideoData");
    ASSERT_NOT_IMPLEMENTED(compResult);
    EXPECT_NE(compResult.data, "");

    auto decompResult = compressor.decompress(compResult.data);
    ASSERT_NOT_IMPLEMENTED(decompResult);
    EXPECT_NE(decompResult.data, "");
}

// Test JPEGCompressor compress and decompress.
TEST(JPEGCompressorTest, CompressAndDecompress) {
    JPEGCompressor compressor;
    auto compResult = compressor.compress("TestImageData");
    ASSERT_NOT_IMPLEMENTED(compResult);
    EXPECT_NE(compResult.data, "");

    auto decompResult = compressor.decompress(compResult.data);
    ASSERT_NOT_IMPLEMENTED(decompResult);
    EXPECT_NE(decompResult.data, "");
}
