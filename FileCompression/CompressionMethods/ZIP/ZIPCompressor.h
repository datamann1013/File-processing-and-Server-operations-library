// Created by data0 on 13.04.2025.

#ifndef ZIP_COMPRESSOR_H
#define ZIP_COMPRESSOR_H

#include "../../ICompressor.h"

// ZIP Compressor
// ZIP is a widely used file archiving format, sometimes subject to patent considerations in certain regions.
// This skeleton serves as the interface for ZIP compression.
class ZIPCompressor : public ICompressor {
public:
    // Compresses inputData using the ZIP algorithm.
    CompressionAPI::CompressionResult compress(const std::string &inputData) override;

    // Decompresses inputData using the ZIP algorithm.
    CompressionAPI::CompressionResult decompress(const std::string &inputData) override;
};

#endif // ZIP_COMPRESSOR_H

