// Created by data0 on 13.04.2025.

#ifndef JPEG_COMPRESSOR_H
#define JPEG_COMPRESSOR_H

#include "../../ICompressor.h"

// JPEG Compressor
// JPEG is a standard for lossy compression of digital images.
// Historically subject to patent royalties, it is now widely used and licensed.
class JPEGCompressor : public ICompressor {
public:
    // Compresses image data using JPEG.
    CompressionAPI::CompressionResult compress(const std::string &inputData) override;

    // Decompresses image data using JPEG.
    CompressionAPI::CompressionResult decompress(const std::string &inputData) override;
};

#endif // JPEG_COMPRESSOR_H
