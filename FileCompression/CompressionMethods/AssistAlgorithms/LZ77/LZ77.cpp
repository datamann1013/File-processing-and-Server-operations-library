// Created by admin on 15.04.2025.

#include "LZ77.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstring>


namespace CompressionAPI {

    constexpr size_t WINDOW_SIZE = 4096;
    constexpr size_t LOOKAHEAD_BUFFER_SIZE = 32;

    // TODO: Keep a copy of the data unaltered till success


    // TODO: Package level checksum



    // Append value in binary to output string
    template<typename T> void appendValue(std::string &output, const T &value) {
        output.append(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    // Read a value from the data
    template <typename T> T readValue(const char* data, size_t &pos) {
        if (pos + sizeof(T) > std::strlen(data)) {
            throw ErrorCodes::Compression::ES9;
        }
        T value;
        std::memcpy(&value, data + pos, sizeof(T));
        pos += sizeof(T);
        return value;
    }


    // TODO: Implement this function to convert your token list into the output format.
    std::string serializeTokens(const std::vector<Token>& tokens, bool includeFileId, bool offset32) {

        std::string output;

        //TODO: Add first two numbers for filename and offset


        // Write header: number of tokens (32-bit unsigned).
        uint32_t tokenCount = static_cast<uint32_t>(tokens.size());
        appendValue(output, tokenCount);

        for (const auto &token : tokens) {

            if (offset32) {
                uint32_t offsetVal = token.offset;
                appendValue(output, offsetVal);
            }
            else {
                uint64_t offsetVal = token.offset;
                appendValue(output, offsetVal);
            }

            uint32_t lengthVal = token.length;
            appendValue(output, lengthVal);

            uint8_t tType = static_cast<uint8_t>(token.type);
            appendValue(output, tType);

            // First its length, then the literal data itself.
            uint32_t literalLen = static_cast<uint32_t>(token.literal.size());
            appendValue(output, literalLen);
            output.append(token.literal);

            // Write file identifier
            if (includeFileId) {
                uint32_t fileIdLen = static_cast<uint32_t>(token.fileIdentifier.size());
                appendValue(output, fileIdLen);
                output.append(token.fileIdentifier);
            }

            uint32_t check = token.checksum;
            appendValue(output, check);
        }
        return output;
    }

    std::vector<Token> deserializeTokens(const std::string& data) {

        std::vector<Token> tokens;
        size_t pos = 0;

        const std::string delimiter = "::";
        size_t firstDelim = data.find(delimiter, pos);
        if (firstDelim == std::string::npos) {
            throw std::runtime_error("Missing start parameter (file identifier flag) - ES9");
        }
        std::string fileIdFlagStr = data.substr(pos, firstDelim - pos);
        pos = firstDelim + delimiter.size();

        size_t secondDelim = data.find(delimiter, pos);
        if (secondDelim == std::string::npos) {
            throw ErrorCodes::Compression::ES15;
        }
        std::string offsetFlagStr = data.substr(pos, secondDelim - pos);
        pos = secondDelim + delimiter.size();

        // Validate flags.
        if ((fileIdFlagStr != "0" && fileIdFlagStr != "1") ||
            (offsetFlagStr != "0" && offsetFlagStr != "1")) {
            throw ErrorCodes::Compression::ES16;
            }
        bool includeFileId = (fileIdFlagStr == "1");
        bool offset32 = (offsetFlagStr == "0");


        if (data.size() < sizeof(uint32_t)) {
            throw ErrorCodes::Compression::ES5;
        }

        uint32_t tokenCount = *reinterpret_cast<const uint32_t*>(data.data());
        pos += sizeof(uint32_t);

        for (uint32_t i = 0; i < tokenCount; ++i) {

            Token token;

            if (offset32) {
                if (pos + sizeof(uint32_t) > data.size())
                    throw ErrorCodes::Compression::ES10;
                uint32_t offsetVal = *reinterpret_cast<const uint32_t*>(data.data() + pos);
                pos += sizeof(uint32_t);
                token.offset = offsetVal;
            } else {
                if (pos + sizeof(uint64_t) > data.size())
                    throw ErrorCodes::Compression::ES10;;
                uint64_t offsetVal = *reinterpret_cast<const uint64_t*>(data.data() + pos);
                pos += sizeof(uint64_t);
                token.offset = offsetVal;
            }

            if (pos + sizeof(uint32_t) > data.size())
                throw ErrorCodes::Compression::ES11;
            token.length = *reinterpret_cast<const uint32_t*>(data.data() + pos);
            pos += sizeof(uint32_t);

            if (pos + sizeof(uint8_t) > data.size())
                throw ErrorCodes::Compression::ES12;
            uint8_t typeVal = *reinterpret_cast<const uint8_t*>(data.data() + pos);
            pos += sizeof(uint8_t);
            token.type = static_cast<Token::TokenType>(typeVal);

            if (pos + sizeof(uint32_t) > data.size())
                throw ErrorCodes::Compression::ES13;
            uint32_t literalLen = *reinterpret_cast<const uint32_t*>(data.data() + pos);
            pos += sizeof(uint32_t);
            if (pos + literalLen > data.size())
                throw ErrorCodes::Compression::ES6;
            token.literal = data.substr(pos, literalLen);
            pos += literalLen;

            // Read file identifier if included.
            if (includeFileId) {
                if (pos + sizeof(uint32_t) > data.size())
                    throw ErrorCodes::Compression::ES14;
                uint32_t fileIdLen = *reinterpret_cast<const uint32_t*>(data.data() + pos);
                pos += sizeof(uint32_t);
                if (pos + fileIdLen > data.size())
                    throw ErrorCodes::Compression::ES7;
                token.fileIdentifier = data.substr(pos, fileIdLen);
                pos += fileIdLen;
            }

            if (pos + sizeof(uint32_t) > data.size())
                throw std::runtime_error("Data too short for checksum.");
            token.checksum = *reinterpret_cast<const uint32_t*>(data.data() + pos);
            pos += sizeof(uint32_t);

            tokens.push_back(token);

            // Read checksum.
            if (pos + sizeof(uint32_t) > data.size()) {
                throw ErrorCodes::Compression::ES8;
            }
            token.checksum = readValue<uint32_t>(data.data(), pos);

            tokens.push_back(token);
        }
        return tokens;
    }


    CompressionResult compressBlob(const std::string& inputData) {
        CompressionResult result;

        size_t inputLength = inputData.size();
        size_t currentPosition = 0;

        // Collects all tokens generated
        std::vector<Token> tokens;

        // Process input data using sliding window / look-ahead buffer
        while (currentPosition < inputLength) {
            size_t windowStart = (currentPosition >= WINDOW_SIZE) ? currentPosition - WINDOW_SIZE : 0;
            size_t currentLookaheadSize = std::min(LOOKAHEAD_BUFFER_SIZE, inputLength - currentPosition);

            // Debug output to show current scanning state.
            std::cout << "Processing position: " << currentPosition << "\n";
            std::cout << "Sliding window range: [" << windowStart << ", " << currentPosition - 1 << "]\n";
            std::cout << "Lookahead buffer size: " << currentLookaheadSize << "\n";

            // Search longest match in sliding window.
            size_t bestMatchLength = 0;
            size_t bestMatchOffset = 0;
            for (size_t searchIndex = windowStart; searchIndex < currentPosition; ++searchIndex) {
                size_t matchLength = 0;
                // Match far from current position.
                while (matchLength < currentLookaheadSize &&
                       inputData[searchIndex + matchLength] == inputData[currentPosition + matchLength]) {
                    ++matchLength;
                    // Prevent accessing beyond current position
                    if (searchIndex + matchLength >= currentPosition)
                        break;
                }
                // Update best match
                if (matchLength > bestMatchLength) {
                    bestMatchLength = matchLength;
                    bestMatchOffset = currentPosition - searchIndex;
                }
            }

            // Token generation
            if (bestMatchLength > 0) {
                std::cout << "Match found: Offset = " << bestMatchOffset
                          << ", Length = " << bestMatchLength << "\n";
                // TODO: Create a token for the match.
                // Optionally include the next symbol after the match if necessary.
                char nextSymbol = (currentPosition + bestMatchLength < inputLength)
                                  ? inputData[currentPosition + bestMatchLength] : '\0';
                tokens.push_back({bestMatchOffset, bestMatchLength, nextSymbol});

                // Advance the currentPosition by the length of the match plus one (if nextSymbol is used).
                // Adjust this based on your token scheme.
                currentPosition += bestMatchLength;
                // If you include the next symbol as a separate literal, you might want to add it and then advance by 1.
            } else {
                // No match found: output a literal token.
                std::cout << "No match, output literal: " << inputData[currentPosition] << "\n";
                tokens.push_back({0, 0, inputData[currentPosition]});
                currentPosition += 1;
            }
        }

        // TODO: Serialize the tokens into the final compressed output.
        // Replace the stub below with your actual serialization logic.
        result.data = serializeTokens(tokens);

        // Indicate a successful operation if tokens were generated.
        // Once fully implemented, check for errors and update errorCode accordingly.
        if (!result.data.empty()) {
            result.errorCode = ErrorCodes::Compression::SUCCESS;
        } else {
            ErrorHandler::logError("CompressionAPI::compressBlob", ErrorCodes::Compression::ES1,
                                     "Compression failed: Empty output generated.");
            result.errorCode = ErrorCodes::Compression::ES1; // Use an appropriate error code
        }

        return result;
    }


    CompressionResult decompressBlob(const std::string& input) {
        CompressionResult result;

        // TODO: Implement LZ77 decompression algorithm.
        // Steps to follow:
        // 1. Parse the stream of tokens (offset, length, literal) produced during compression.
        // 2. Reconstruct the original data using a sliding window.
        // 3. Log errors using ErrorHandler::logError if data integrity is compromised.

        ErrorHandler::logError("CompressionAPI::decompressBlob", ErrorCodes::Compression::ENN99,
                                 "decompressBlob: Not implemented.");
        result.errorCode = ErrorCodes::Compression::ENN99;
        result.data = ""; // No decompressed data produced.

        return result;
    }
}
