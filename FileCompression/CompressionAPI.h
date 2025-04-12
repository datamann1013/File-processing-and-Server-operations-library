// Created by data0 on 12.04.2025.

#ifndef COMPRESSION_API_H
#define COMPRESSION_API_H

#include <string>
#include "../errorhandler/ErrorCodes.h"  // Adjust path as necessary.

namespace CompressionAPI {

    // Structure to hold the result of a compression operation.
    struct CompressionResult {
        ErrorCodes::Compression errorCode;  // Error code from the compression operation.
        std::string compressedData;          // Placeholder for compressed data.
    };

    // Function to compress a blob of files provided as a JSON string.
    // Currently a stub that returns dummy compressed data.
    CompressionResult compressBlob(const std::string& jsonInput);

    // Function to decompress a compressed blob provided as a JSON string.
    // Stub implementation for demonstration purposes.
    CompressionResult decompressBlob(const std::string& jsonInput);

}

#endif // COMPRESSION_API_H
