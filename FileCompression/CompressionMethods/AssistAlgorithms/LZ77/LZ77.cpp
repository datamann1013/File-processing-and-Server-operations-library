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
        ErrorHandler::logInfo("LZ77::appendValue", ErrorCodes::Compression::IC1, "appendValue start");
        output.append(reinterpret_cast<const char*>(&value), sizeof(T));
        ErrorHandler::logInfo("LZ77::appendValue", ErrorCodes::Compression::IC2, "appendValue complete");
    }

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


    std::string serializeTokens(const std::vector<Token>& tokens, bool includeFileId, bool offset32) {

        ErrorHandler::logInfo("LZ77::serializeTokens", ErrorCodes::Compression::IC3, "serializeTokens start");
        std::string output;

        std::string header = std::to_string(includeFileId ? 1 : 0) + "::" + std::to_string(offset32 ? 0 : 1) + "::";
        output.append(header);


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

            appendValue(output, token.length);

            appendValue(output, static_cast<uint8_t>(token.type));

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

            appendValue(output, token.checksum);
            ErrorHandler::logInfo("LZ77::serializeTokens", ErrorCodes::Compression::IC4, "serializeTokens complete");
        }
        return output;
    }

    std::vector<Token> deserializeTokens(const std::string& data) {
        ErrorHandler::logInfo("LZ77::deserializeTokens", ErrorCodes::Compression::IC5, "deserializeTokens start");
        std::vector<Token> tokens;
        size_t pos = 0;
        size_t totalSize = data.size();
        // Parse header
        const std::string delim = "::";
        auto f = data.find(delim, pos);
        if (f == std::string::npos) {
            ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES15, "Header parameters are missing");
            throw ErrorCodes::Compression::ES15;
        }
        bool includeID = (data.substr(0,f) == "1");
        pos = f + 2;
        f = data.find(delim, pos);
        if (f == std::string::npos) {
            ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES15, "Header parameters are missing");
            throw ErrorCodes::Compression::ES15;
        }
        bool offset32 = (data.substr(pos,f-pos) == "1");
        pos = f + 2;
        // Token count
        uint32_t count = *reinterpret_cast<const uint32_t*>(data.data()+pos);
        pos += sizeof(uint32_t);
        for (uint32_t i=0; i<count; ++i) {
            Token t;
            // Offset
            if (offset32) { t.offset = *reinterpret_cast<const uint32_t*>(data.data()+pos); pos+=4; }
            else { t.offset = *reinterpret_cast<const uint64_t*>(data.data()+pos); pos+=8; }
            // Length
            t.length = *reinterpret_cast<const uint32_t*>(data.data()+pos); pos+=4;
            // Type
            t.type = static_cast<Token::TokenType>(data[pos]); pos+=1;
            // Literal
            uint32_t ll = *reinterpret_cast<const uint32_t*>(data.data()+pos); pos+=4;
            t.literal = data.substr(pos, ll); pos+=ll;
            // File ID
            if (includeID) {
                uint32_t idl = *reinterpret_cast<const uint32_t*>(data.data()+pos); pos+=4;
                t.fileIdentifier = data.substr(pos, idl); pos+=idl;
            }
            // Checksum
            t.checksum = *reinterpret_cast<const uint32_t*>(data.data()+pos); pos+=4;
            tokens.push_back(t);
        }
        ErrorHandler::logInfo("LZ77::deserializeTokens", ErrorCodes::Compression::IC6, "deserializeTokens complete");
        return tokens;
    }


    CompressionResult compressBlob(const std::string& inputData, bool includeID, bool offset32) {
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
