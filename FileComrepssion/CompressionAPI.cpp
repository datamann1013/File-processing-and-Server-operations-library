// Created by data0 on 12.04.2025.

#include "CompressionAPI.h"

#include "../Errorhandler/ErrorHandler.h" // Adjust relative path as needed.
#include <iostream>

namespace CompressionAPI {

    CompressionResult compressBlob(const std::string& jsonInput) {
        CompressionResult result;

        // Validate JSON input (stub: if empty, consider it invalid).
        if (jsonInput.empty()) {
            result.errorCode = ErrorCodes::Compression::INVALID_JSON;
            ErrorHandler::logError("CompressionAPI", static_cast<int>(result.errorCode), "Empty JSON input.");
            return result;
        }

        // In a full implementation: Parse JSON, select algorithm, perform compression, calculate checksum, etc.
        result.errorCode = ErrorCodes::Compression::SUCCESS;
        result.compressedData = "DummyCompressedData";  // Simulated output.
        return result;
    }

}// namespace CompressionAPI