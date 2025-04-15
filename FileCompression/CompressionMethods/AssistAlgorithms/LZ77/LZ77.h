// Created by admin on 15.04.2025.

#ifndef LZ77_H
#define LZ77_H

#include <string>
#include "../../../../Errorhandler/ErrorCodes.h"
#include "../../../../Errorhandler/ErrorHandler.h"

namespace CompressionAPI {

    // Structure to hold the result from the compression/decompression functions.
    struct CompressionResult {
        ErrorCodes::Compression errorCode;
        std::string data; // Compressed or decompressed data.
    };

    /**
     * @param input The input data to compress.
     * @return CompressionResult with compressed data and an error code.
     */
    CompressionResult compressBlob(const std::string& input);

    /**
     * @param input The compressed data to decompress.
     * @return CompressionResult with decompressed data and an error code.
     */
    CompressionResult decompressBlob(const std::string& input);
}

#endif // LZ77_H
