// Created by data0 on 12.04.2025.

#ifndef COMPRESSION_API_H
#define COMPRESSION_API_H

#include <string>
#include "../errorhandler/ErrorCodes.h"  // Ensure the path is correct.

namespace CompressionAPI {

    // Structure to hold the result of compression or decompression.
    // When fully implemented, the functions will check for errors such as:
    // - JSON Parsing errors,
    // - Unsupported file types,
    // - Invalid precompression parameters,
    // - Missing metadata,
    // - Checksum mismatches,
    // - Encryption requests, etc.
    // For now, every function returns the universal "Not Implemented" code: ENN99.
    struct CompressionResult {
        ErrorCodes::Compression errorCode;           // Error code (using the constants from ErrorCodes.h)
        std::string data;        // Compressed or decompressed data.
    };

    // Compresses a blob of files provided in a JSON string.
    // Stub implementation: should:
    // 1. Parse the JSON input.
    // 2. Validate file metadata (lastModified, creationDate).
    // 3. Check for supported file types.
    // 4. Choose the proper compression algorithm (default to LZMA if none specified).
    // 5. Process precompression parameters.
    // 6. Handle encryption flags (if set, warn user).
    // 7. Return the compressed result.
    // Currently, returns ENN99 (not implemented).
    CompressionResult compressBlob(const std::string& jsonInput);

    // Decompresses a blob based on a JSON input.
    // Stub implementation: should:
    // 1. Parse the JSON input.
    // 2. Extract compressed data.
    // 3. Validate checksums and seals.
    // 4. Return the decompressed result.
    // Currently, returns ENN99 (not implemented).
    CompressionResult decompressBlob(const std::string& jsonInput);
}

#endif // COMPRESSION_API_H
