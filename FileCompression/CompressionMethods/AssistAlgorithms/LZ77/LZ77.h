// Created by admin on 15.04.2025.

#ifndef LZ77_H
#define LZ77_H

#include <cstdint>
#include <string>
#include "../../../../Errorhandler/ErrorCodes.h"
#include "../../../../Errorhandler/ErrorHandler.h"

namespace CompressionAPI {

    // Structure to hold the result from the compression/decompression functions.
    struct CompressionResult {
        ErrorCodes::Compression errorCode;
        std::string data; // Compressed or decompressed data.
    };

    struct Token {
        // Specify position in input from where the match begins.
        uint32_t  offset;
        uint32_t  length;

        // Stores literal data that follows the match
        std::string literal;

        // For cross-checking integrity
        std::string fileIdentifier;

        // Checksum for this token's data
        uint32_t checksum;

        // Indicates what kind of token this is. Helps the decompressor interpret the token correctly.
        enum class TokenType : uint8_t {
            LITERAL = 0,     // No match found
            MATCH = 1,       // With offset and length
            END_OF_BLOCK = 2, // End marker.
            CONTROL = 3       // For special control commands
        } type;
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
