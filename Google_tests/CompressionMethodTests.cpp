// Created by data0 on 13.04.2025.

#include <gtest/gtest.h>
#include "../FileCompression/CompressionMethods/LZMA/LZMACompressor.h"
#include "../FileCompression/CompressionMethods/ZIP/ZIPCompressor.h"
#include "../FileCompression/CompressionMethods/H264/H264Compressor.h"
#include "../FileCompression/CompressionMethods/JPEG/JPEGCompressor.h"
#include "../errorhandler/ErrorCodes.h"

#define ASSERT_NOT_IMPLEMENTED(result)                    \
    if (result.errorCode == ErrorCodes::Compression::ENN99) { \
        FAIL() << "Function not implemented (returned ENN99)";  \
    }


TEST(LZMACompressorTest, CompressAndDecompress) {
    LZMACompressor compressor;

    auto compResult = compressor.compress("TestData");
    ASSERT_NOT_IMPLEMENTED(compResult);
    EXPECT_EQ(compResult.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_NE(compResult.data, ""); // Expect non-empty compressed data.

    auto decompResult = compressor.decompress(compResult.data);
    ASSERT_NOT_IMPLEMENTED(decompResult);
    EXPECT_EQ(decompResult.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_NE(decompResult.data, "");
}

TEST(ZIPCompressorTest, CompressAndDecompress) {
    ZIPCompressor compressor;

    auto compResult = compressor.compress("TestData");
    ASSERT_NOT_IMPLEMENTED(compResult);
    EXPECT_EQ(compResult.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_NE(compResult.data, "");

    auto decompResult = compressor.decompress(compResult.data);
    ASSERT_NOT_IMPLEMENTED(decompResult);
    EXPECT_EQ(decompResult.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_NE(decompResult.data, "");
}

TEST(H264CompressorTest, CompressAndDecompress) {
    H264Compressor compressor;

    auto compResult = compressor.compress("TestVideoData");
    ASSERT_NOT_IMPLEMENTED(compResult);
    EXPECT_EQ(compResult.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_NE(compResult.data, "");

    auto decompResult = compressor.decompress(compResult.data);
    ASSERT_NOT_IMPLEMENTED(decompResult);
    EXPECT_EQ(decompResult.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_NE(decompResult.data, "");
}


TEST(JPEGCompressorTest, CompressAndDecompress) {
    JPEGCompressor compressor;

    auto compResult = compressor.compress("TestImageData");
    ASSERT_NOT_IMPLEMENTED(compResult);
    EXPECT_EQ(compResult.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_NE(compResult.data, "");

    auto decompResult = compressor.decompress(compResult.data);
    ASSERT_NOT_IMPLEMENTED(decompResult);
    EXPECT_EQ(decompResult.errorCode, ErrorCodes::Compression::SUCCESS);
    EXPECT_NE(decompResult.data, "");
}
