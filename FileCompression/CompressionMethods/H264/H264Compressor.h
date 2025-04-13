// Created by data0 on 13.04.2025.

#ifndef H264_COMPRESSOR_H
#define H264_COMPRESSOR_H

#include "../../ICompressor.h"

// H.264 Compressor
// H.264 is a video compression standard widely used for high-definition video.
// Note: H.264 is subject to various patents and licensing, typically managed by MPEG LA.
class H264Compressor : public ICompressor {
public:
    // Compresses input video data using H.264.
    CompressionAPI::CompressionResult compress(const std::string &inputData) override;

    // Decompresses input video data using H.264.
    CompressionAPI::CompressionResult decompress(const std::string &inputData) override;
};

#endif // H264_COMPRESSOR_H
