// Created by data0 on 13.04.2025.

#ifndef ZIP_COMPRESSOR_H
#define ZIP_COMPRESSOR_H

#include "../../ICompressor.h"

// ZIP Compressor
class ZIPCompressor : public ICompressor {
public:
    // Compresses inputData using the ZIP algorithm.
    CompressionAPI::CompressionResult compress(const std::string &inputData) override;

    // Decompresses inputData using the ZIP algorithm.
    CompressionAPI::CompressionResult decompress(const std::string &inputData) override;
};

#endif // ZIP_COMPRESSOR_H

