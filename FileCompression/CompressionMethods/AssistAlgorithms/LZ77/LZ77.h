// Created by admin on 15.04.2025.

#ifndef LZ77_H
#define LZ77_H

#include <cstdint>
#include <string>
#include <cstring>
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
    std::string serializeTokens(const std::vector<Token>& tokens, bool includeFileId, bool use32BitOffset);

    // Parameters:
    //  - data: The binary string containing the serialized tokens.
    //
    // Throws std::runtime_error if the data is too short.
    std::vector<Token> deserializeTokens(const std::string& data);


    /**
     * @param input The input data to compress.
     * @return CompressionResult with compressed data and an error code.
     */
    CompressionResult compressBlob(const std::string& input,bool includeID, bool offset32);

    /**
     * @param input The compressed data to decompress.
     * @return CompressionResult with decompressed data and an error code.
     */
    CompressionResult decompressBlob(const std::string& input);

    // Read a value from the data
    template<typename T> T readValue(const char* data, size_t &pos, size_t totalSize) {
        ErrorHandler::logInfo("LZ77::readValue", ErrorCodes::Compression::IC1, "readValue start");
        if (pos + sizeof(T) > totalSize) {
            ErrorHandler::logError("LZ::readValue", ErrorCodes::Compression::ES9, "readValue too big");
            throw ErrorCodes::Compression::ES9;
        }
        T value;
        std::memcpy(&value, data + pos, sizeof(T));
        pos += sizeof(T);
        ErrorHandler::logInfo("LZ77::readValue", ErrorCodes::Compression::IC2, "readValue complete");
        return value;
    }
    // Append value in binary to output string
    template<typename T> void appendValue(std::string &output, const T &value) {
        ErrorHandler::logInfo("LZ77::appendValue", ErrorCodes::Compression::IC1, "appendValue start");
        output.append(reinterpret_cast<const char*>(&value), sizeof(T));
        ErrorHandler::logInfo("LZ77::appendValue", ErrorCodes::Compression::IC2, "appendValue complete");
    }

    inline bool operator==(const Token &lhs, const Token &rhs) {
        return lhs.offset         == rhs.offset
            &&  lhs.length         == rhs.length
            &&  lhs.literal        == rhs.literal
            &&  lhs.fileIdentifier == rhs.fileIdentifier
            &&  lhs.checksum       == rhs.checksum
            &&  lhs.type           == rhs.type;
    }

}

#endif // LZ77_H
