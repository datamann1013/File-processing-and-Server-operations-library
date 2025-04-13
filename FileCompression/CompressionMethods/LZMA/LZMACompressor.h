// Created by data0 on 13.04.2025.

#ifndef LZMA_COMPRESSOR_H
#define LZMA_COMPRESSOR_H

#include "../../ICompressor.h"

// LZMA Compressor
// LZMA is an open-source compression algorithm known for its high compression ratio.
// Note: LZMA is covered under the LZMA SDK license.
class LZMACompressor : public ICompressor {
public:
    // Compresses inputData using LZMA.
    // Future implementation should apply the LZMA algorithm.
    CompressionAPI::CompressionResult compress(const std::string &inputData) override;

    // Decompresses inputData using LZMA.
    // Future implementation should apply the LZMA decompression algorithm.
    CompressionAPI::CompressionResult decompress(const std::string &inputData) override;
};

#endif // LZMA_COMPRESSOR_H
