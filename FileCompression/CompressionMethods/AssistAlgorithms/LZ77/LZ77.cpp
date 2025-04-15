// Created by admin on 15.04.2025.

#include "LZ77.h"
#include <string>
#include <iostream>

namespace CompressionAPI {

    constexpr size_t WINDOW_SIZE = 4096;
    constexpr size_t LOOKAHEAD_BUFFER_SIZE = 32;

    CompressionResult compressBlob(const std::string& inputData) {
        CompressionResult result;

        size_t inputLength = inputData.size();
        size_t currentPosition = 0;

        // The sliding window is implicitly defined by a range within the input data.
        // At each step, the window is the segment from:
        //    max(0, currentPosition - WINDOW_SIZE) to currentPosition - 1.
        // The look-ahead buffer starts at currentPosition and extends over the next LOOKAHEAD_BUFFER_SIZE bytes.

        while (currentPosition < inputLength) {
            // Define the starting point of the sliding window
            size_t windowStart = (currentPosition >= WINDOW_SIZE) ? currentPosition - WINDOW_SIZE : 0;

            // Determine the current look-ahead buffer size (could be less near the end of input)
            size_t currentLookaheadSize = std::min(LOOKAHEAD_BUFFER_SIZE, inputLength - currentPosition);

            // Debug: output current positions
            std::cout << "Processing position: " << currentPosition << "\n";
            std::cout << "Sliding window range: [" << windowStart << ", " << currentPosition - 1 << "]\n";
            std::cout << "Lookahead buffer size: " << currentLookaheadSize << "\n";

            // Example: search for the longest match in the window
            size_t bestMatchLength = 0;
            size_t bestMatchOffset = 0;
            for (size_t searchIndex = windowStart; searchIndex < currentPosition; ++searchIndex) {
                size_t matchLength = 0;
                // Try to match as far as possible from the current position.
                while (matchLength < currentLookaheadSize &&
                       inputData[searchIndex + matchLength] == inputData[currentPosition + matchLength]) {
                    ++matchLength;
                    // Prevent accessing out of the sliding window boundary.
                    if (searchIndex + matchLength >= currentPosition)
                        break;
                }
                // Update best match if current one is longer.
                if (matchLength > bestMatchLength) {
                    bestMatchLength = matchLength;
                    bestMatchOffset = currentPosition - searchIndex;
                }
            }

            // Use the match information to decide what to output.
            if (bestMatchLength > 0) {
                std::cout << "Match found: Offset = " << bestMatchOffset
                          << ", Length = " << bestMatchLength << "\n";
                // In a full implementation, output a token and move currentPosition forward.
                currentPosition += bestMatchLength;
            } else {
                // No match found, output the literal.
                std::cout << "No match, output literal: " << inputData[currentPosition] << "\n";
                currentPosition += 1;
            }
        }
        // TODO: Implement LZ77 compression algorithm.
        // Steps to follow:
        // 1. Initialize sliding window and look-ahead buffer.
        // 2. For each position in input:
        //      - Search for the longest match in the window.
        //      - Output a token (offset, length, next symbol) if a match exists.
        //      - Otherwise, output the literal.
        // 3. Advance the window and repeat.
        // 4. Log any errors using ErrorHandler::logError if required.

        ErrorHandler::logError("CompressionAPI::compressBlob", ErrorCodes::Compression::ENN99,
                                 "compressBlob: Not implemented.");
        result.errorCode = ErrorCodes::Compression::ENN99;
        result.data = ""; // No compressed data produced.

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
