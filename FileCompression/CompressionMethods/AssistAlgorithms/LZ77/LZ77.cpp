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

        output += std::string(1, includeFileId ? '1' : '0') + "::";
        output += std::string(1, use32BitOffset ? '1' : '0') + "::";


        // Token count
        appendValue<uint32_t>(output, static_cast<uint32_t>(tokens.size()));

        for (const auto &token : tokens) {

            if (use32BitOffset)
                appendValue<uint32_t>(output, static_cast<uint32_t>(token.offset));
            else {
                appendValue<uint64_t>(output, token.offset);
            }

            appendValue<uint32_t>(output, token.length);
            appendValue(output, static_cast<uint8_t>(token.type));

            // Literal length and data
            appendValue<uint32_t>(output, static_cast<uint32_t>(token.literal.size()));
            output.append(token.literal);

            // Write file identifier
            if (includeFileId) {
                appendValue<uint32_t>(output, static_cast<uint32_t>(token.fileIdentifier.size()));
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
        size_t pos = 0, totalSize = data.size();

        // Parse headerflags

        auto readBoolFlag = [&](bool &flag) {
            size_t sep = data.find("::", pos);
            if (sep == std::string::npos || sep >= totalSize)
                throw ErrorCodes::Compression::ES15;
            ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES15, "Header parameters are missing");
            flag = data[pos] == '1';
            pos = sep + 2;
        };

        bool includeId, offset32;
        readBoolFlag(includeId);
        readBoolFlag(offset32);


        // Token count
        uint32_t count = readValue<uint32_t>(data.data(), pos, totalSize);
        tokens.reserve(count);

        for (uint32_t i=0; i<count; ++i) {
            Token t;

           // Offset
        t.offset = offset32
            ? readValue<uint32_t>(data.data(), pos, totalSize)
            : readValue<uint64_t>(data.data(), pos, totalSize);

            // Length
            t.length = readValue<uint32_t>(data.data(), pos, totalSize);

            // Type
            t.type = static_cast<Token::TokenType>(readValue<uint8_t>(data.data(), pos, totalSize));

            // Literal
            uint32_t litLen = readValue<uint32_t>(data.data(), pos, totalSize);
            if (pos + litLen > totalSize) {
                ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES17, "Truncated literal data");
                throw ErrorCodes::Compression::ES17;

            }
            t.literal = data.substr(pos, litLen);
            pos += litLen;

            // File ID
            if (includeId) {
                uint32_t idLen = readValue<uint32_t>(data.data(), pos, totalSize);
                if (pos + idLen > totalSize) {
                    ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES17, "Truncated fileIdentifier data");
                    throw ErrorCodes::Compression::ES17;
                }
                t.fileIdentifier = data.substr(pos, idLen);
                pos += idLen;
            }

            t.checksum = readValue<uint32_t>(data.data(), pos, totalSize);
            tokens.push_back(std::move(t));

        }

        ErrorHandler::logInfo("LZ77::deserializeTokens", ErrorCodes::Compression::IC6, "deserializeTokens complete");
        return tokens;
    }


    CompressionResult compressBlob(const std::string& input, const bool includeID, const bool offset32) {
        CompressionResult result;

        size_t inputLength = input.size(), currentPosition = 0;
        std::vector<Token> tokens;

        // Process input data using sliding window / look-ahead buffer
        while (currentPosition < inputLength) {
            size_t windowStart = (currentPosition >= WINDOW_SIZE) ? currentPosition - WINDOW_SIZE : 0;
            size_t currentLookaheadSize = std::min(LOOKAHEAD_BUFFER_SIZE, inputLength - currentPosition);

            // Search longest match in sliding window.
            size_t bestMatchLength = 0, bestMatchOffset = 0;
            for (size_t searchIndex = windowStart; searchIndex < currentPosition; ++searchIndex) {
                size_t matchLength = 0;

                while (matchLength < currentLookaheadSize &&
                       input[searchIndex + matchLength] == input[currentPosition + matchLength]) {
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
                                  ? input[currentPosition + bestMatchLength] : '\0';
               //TODO: tokens.push_back({bestMatchOffset, bestMatchLength, nextSymbol});

                // Advance the currentPosition by the length of the match plus one (if nextSymbol is used).
                // Adjust this based on your token scheme.
                currentPosition += bestMatchLength;
                // If you include the next symbol as a separate literal, you might want to add it and then advance by 1.
            } else {
                // No match found: output a literal token.
                std::cout << "No match, output literal: " << input[currentPosition] << "\n";
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
