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



    // Append value in binary to output string
    template<typename T>
    void appendValue(std::string &output, const T &value) {
        output.append(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    // Read a value from the data
    template<typename T>
    T readValue(const char* data, size_t& pos) {
        T value;
        std::memcpy(&value, data + pos, sizeof(T));
        pos += sizeof(T);
        return value;
    }

    // TODO: Implement this function to convert your token list into the output format.
    std::string serializeTokens(const std::vector<Token>& tokens) {
        // As an example, you may choose to simply append the token values into a string.
        // In practice, you'd encode these values in a binary or a more efficient format.
        std::string output;
        for (const auto& token : tokens) {
            output += "{" + std::to_string(token.offset) + "," +
                      std::to_string(token.length) + "," +
                      token.nextSymbol + "}";
        }
        return output;
    }

    std::vector<Token> deserializeTokens(const std::string& data, bool includeFileId) {

        std::vector<Token> tokens;

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
