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
        uint64_t  offset;
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

    // Parameters:
    //  - tokens: The vector of tokens to serialize.
    //  - includeFileId: If true, include the fileIdentifier field in the output.
    //  - offset32: If true, serialize the offset as a 32-bit value (uint32_t); otherwise use 64-bit (uint64_t).


    //  [tokenCount (uint32_t)]
    //  For each token:
    //    - offset (uint32_t if offset32=true, else uint64_t)
    //    - length (uint32_t)
    //    - token type (uint8_t)
    //    - literal length (uint32_t)
    //    - literal data (literal length bytes)
    //    - If includeFileId is true:
    //         - file identifier length (uint32_t)
    //         - file identifier (that many bytes)
    //    - checksum (uint32_t)

    // TODO: Consider adding a delimiter (e.g., "::") between literal length and literal data for readability if needed.
    // TODO: Consider compressing fields further (e.g., varint encoding) for space efficiency.
    std::string serializeTokens(const std::vector<Token>& tokens, bool includeFileId, bool offset32);

    // Parameters:
    //  - data: The binary string containing the serialized tokens.
    //
    // Throws std::runtime_error if the data is too short.
    std::vector<Token> deserializeTokens(const std::string& data);


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
