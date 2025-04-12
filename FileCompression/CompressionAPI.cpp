// Created by data0 on 12.04.2025.

#include "CompressionAPI.h"
#include "../errorhandler/ErrorHandler.h"
#include <iostream>

namespace CompressionAPI {

    CompressionResult compressBlob(const std::string& jsonInput) {
        CompressionResult result;

        if (jsonInput.empty()) {
            result.errorCode = ErrorCodes::Compression::EU1; // invalid JSON.
            ErrorHandler::logError("CompressionAPI", result.errorCode, "compressBlob: Empty JSON input provided.");
            return result;
        }

        // In a full implementation:
        // 1. Parse JSON input.
        // 2. Validate file metadata (fileName, fileType, fileData, lastModified, creationDate).
        // 3. Determine compression algorithm:
        //    - Use specified algorithm if valid and compatible.
        //    - Otherwise, default to LZMA.
        // 4. If encryption is requested, warn (WU2) since it's not implemented.
        // 5. Perform compression and calculate checksum.
        // 6. Return the compressed data or an error if something fails.

        // Here, we simulate a successful compression.
        result.errorCode = ErrorCodes::Compression::SUCCESS;
        result.compressedData = "DummyCompressedData";
        return result;
    }

    CompressionResult decompressBlob(const std::string& jsonInput) {
        CompressionResult result;

        // Check for empty input.
        if (jsonInput.empty()) {
            result.errorCode = ErrorCodes::Compression::EU1; // User-side, invalid JSON.
            ErrorHandler::logError("CompressionAPI", result.errorCode, "decompressBlob: Empty JSON input provided.");
            return result;
        }

        // In a full implementation, perform steps similar to compression:
        // 1. Parse JSON input for compressed data.
        // 2. Validate checksums and seals.
        // 3. Decompress data.
        // 4. Return decompressed content or an error.

        // For demonstration, simulate a successful decompression.
        result.errorCode = ErrorCodes::Compression::SUCCESS;
        result.compressedData = "DummyDecompressedData";
        return result;
    }

} // namespace CompressionAPI
