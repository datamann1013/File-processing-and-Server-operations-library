// Created by data0 on 13.04.2025.

#ifndef I_COMPRESSOR_H
#define I_COMPRESSOR_H

#include <string>
#include "../errorhandler/ErrorCodes.h"

namespace CompressionAPI {

    // The result structure for compression operations.
    struct CompressionResult {
        ErrorCodes::Compression errorCode;  // The error code (e.g., SUCCESS, or specific error).
        std::string data;                    // Output data (compressed or decompressed).
    };

} // namespace CompressionAPI

// Common interface for all compression method implementations.
class ICompressor {
public:
    virtual ~ICompressor() {}
    // Compresses the inputData and returns the result.
    virtual CompressionAPI::CompressionResult compress(const std::string &inputData) = 0;
    // Decompresses the inputData and returns the result.
    virtual CompressionAPI::CompressionResult decompress(const std::string &inputData) = 0;
};

#endif // I_COMPRESSOR_H
