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


    std::string serializeTokens(const std::vector<Token>& tokens, bool includeFileId, bool use32BitOffset) {

        ErrorHandler::logInfo("LZ77::serializeTokens", ErrorCodes::Compression::IC3, "serializeTokens start");
        std::string output;

        std::string header = std::to_string(includeFileId ? 1 : 0) + "::" + std::to_string(use32BitOffset ? 1 : 0) + "::";
        output.append(header);


        // Write header: number of tokens (32-bit unsigned).
        uint32_t tokenCount = static_cast<uint32_t>(tokens.size());
        appendValue(output, tokenCount);

        for (const auto &token : tokens) {

            if (use32BitOffset) {
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

        // Parse headerflags
        const std::string delim = "::";
        auto f = data.find(delim, pos);
        if (f == std::string::npos) {
            ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES15, "Header parameters are missing");
            throw ErrorCodes::Compression::ES15;
        }

        bool includeID = (data.substr(pos, f - pos) == "1");
        pos = f + delim.size();

        f = data.find(delim, pos);
        if (f == std::string::npos) {
            ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES15, "Header parameters are missing");
            throw ErrorCodes::Compression::ES15;
        }

        bool offset32 = (data.substr(pos, f - pos) == "1");
        pos = f + delim.size();


        // Token count
        uint32_t count = readValue<uint32_t>(data.data(), pos, totalSize);

        for (uint32_t i=0; i<count; ++i) {
            Token t;

            // Offset
            if (offset32) {
                t.offset = readValue<uint32_t>(data.data(), pos, totalSize);
            }
            else {
                t.offset = readValue<uint64_t>(data.data(), pos, totalSize);
            }

            // Length
            t.length = readValue<uint32_t>(data.data(), pos, totalSize);

            // Type
            uint8_t rawType = readValue<uint8_t>(data.data(), pos, totalSize);
            t.type = static_cast<Token::TokenType>(rawType);

            // Literal
            uint32_t litLen = readValue<uint32_t>(data.data(), pos, totalSize);
            if (pos + litLen > totalSize) {
                ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES17, "Truncated literal data");
                throw ErrorCodes::Compression::ES17;

            }
            t.literal = data.substr(pos, litLen);
            pos += litLen;

            // File ID
            if (includeID) {
                uint32_t idLen = readValue<uint32_t>(data.data(), pos, totalSize);
                if (pos + idLen > totalSize) {
                    ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES17, "Truncated fileIdentifier data");
                    throw ErrorCodes::Compression::ES17;
                }
                t.fileIdentifier = data.substr(pos, idLen);
                pos += idLen;
            }

            t.checksum = readValue<uint32_t>(data.data(), pos, totalSize);
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
               //TODO: tokens.push_back({bestMatchOffset, bestMatchLength, nextSymbol});

                // Advance the currentPosition by the length of the match plus one (if nextSymbol is used).
                // Adjust this based on your token scheme.
                currentPosition += bestMatchLength;
                // If you include the next symbol as a separate literal, you might want to add it and then advance by 1.
            } else {
                // No match found: output a literal token.
                std::cout << "No match, output literal: " << inputData[currentPosition] << "\n";
               //TODO: tokens.push_back({0, 0, inputData[currentPosition]});
                currentPosition += 1;
            }
        }

        // TODO: Serialize the tokens into the final compressed output.
        // Replace the stub below with your actual serialization logic.
        result.data = serializeTokens(tokens, includeID, offset32);

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
