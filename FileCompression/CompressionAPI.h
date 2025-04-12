// Created by data0 on 12.04.2025.

#ifndef COMPRESSION_API_H
#define COMPRESSION_API_H

#include <string>
#include "../errorhandler/ErrorCodes.h"

namespace CompressionAPI {

    // Structure to hold the results of a compression or decompression operation.
    struct CompressionResult {
        ErrorCodes::Compression errorCode;  // Holds an error code.
        std::string compressedData;          // The output data (or decompressed data).
    };

    // Compresses a blob of files described by a JSON string.
    // A stub implementation for demonstration. In a full implementation, this
    // will parse JSON, select a compression algorithm, perform compression, etc.
    CompressionResult compressBlob(const std::string& jsonInput);

    // Decompresses a compressed blob described by a JSON string.
    // A stub implementation for demonstration.
    CompressionResult decompressBlob(const std::string& jsonInput);

    // Future: Additional functions for specific compression algorithms can be added here.
}

#endif // COMPRESSION_API_H
