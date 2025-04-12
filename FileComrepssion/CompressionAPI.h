// Created by data0 on 12.04.2025.

#ifndef COMPRESSIONAPI_H
#define COMPRESSIONAPI_H

#include <string>
#include "../Errorhandler/ErrorCodes.h"

namespace CompressionAPI {

    // Structure holding the compression result.
    struct CompressionResult {
        ErrorCodes::Compression errorCode;  // Error code from the compression operation.
        std::string compressedData;          // Placeholder for the compressed data.
    };

    // Function to compress a blob of files provided as a JSON string.
    // This stub implementation returns dummy compressed data.
    CompressionResult compressBlob(const std::string& jsonInput);
}

#endif //COMPRESSIONAPI_H
