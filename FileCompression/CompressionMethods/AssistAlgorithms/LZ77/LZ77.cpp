// Created by admin on 15.04.2025.

#include "LZ77.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstring>


namespace CompressionAPI {
    constexpr size_t WINDOW_SIZE = 4096;
    constexpr size_t LOOKAHEAD_BUFFER_SIZE = 32;


    std::string serializeTokens(const std::vector<Token>& tokens, bool includeFileId,
        bool use32BitOffset, uint32_t packageID, uint32_t globalChecksum) {



        ErrorHandler::logInfo("LZ77::serializeTokens", ErrorCodes::Compression::IC3,
            "serializeTokens start");

        std::string output;

        output += std::string(1, includeFileId ? '1' : '0') + "::";
        output += std::string(1, use32BitOffset ? '1' : '0') + "::";
        appendValue<uint32_t>(output, packageID);
        output += "::";
        appendValue<uint32_t>(output, static_cast<uint32_t>(tokens.size()));
        output += "::";



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
            appendValue<uint32_t>(output, token.checksum);
        }

        output += "::";
        appendValue<uint32_t>(output, globalChecksum);

        ErrorHandler::logInfo("LZ77::serializeTokens", ErrorCodes::Compression::IC4,
            "serializeTokens complete");
        return output;
    }

    std::vector<Token> deserializeTokens(const std::string& data) {

        ErrorHandler::logInfo("LZ77::deserializeTokens", ErrorCodes::Compression::IC5,
            "deserializeTokens start");

        std::vector<Token> tokens;
        size_t pos = 0, totalSize = data.size();
        // Parse header flags

        constexpr size_t MIN_HEADER =
        /* flag */       1 + 2
      + /* flag */       1 + 2
      + /* uint32_t */   sizeof(uint32_t) + 2
      + /* uint32_t */   sizeof(uint32_t) + 2;

        if (totalSize < MIN_HEADER) {
            ErrorHandler::logError("LZ77::deserializeTokens",
                ErrorCodes::Compression::ES15,
                "Input too short for header (need at least "
                + std::to_string(MIN_HEADER) + " bytes, got "
                + std::to_string(totalSize) + ")");
            throw ErrorCodes::Compression::ES15;
        }

        auto readBoolFlag = [&](bool &flag) {
            size_t sep = data.find("::", pos);
            if (sep == std::string::npos || sep >= totalSize) {
                ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES15,
                    "Header parameters are missing");
                throw ErrorCodes::Compression::ES15;
            }

            flag = data[pos] == '1';
            pos = sep + 2;
        };

        bool includeId, offset32;
        readBoolFlag(includeId);
        readBoolFlag(offset32);


        // Token count
        auto count = readValue<uint32_t>(data.data(), pos, totalSize);
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
            auto litLen = readValue<uint32_t>(data.data(), pos, totalSize);
            if (pos + litLen > totalSize) {
                ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES17,
                    "Truncated literal data");
                throw ErrorCodes::Compression::ES17;
            }
            t.literal = data.substr(pos, litLen);
            pos += litLen;

            // File ID
            if (includeId) {
                auto idLen = readValue<uint32_t>(data.data(), pos, totalSize);
                if (pos + idLen > totalSize) {
                    ErrorHandler::logError("LZ77::deserializeTokens", ErrorCodes::Compression::ES17,
                        "Truncated fileIdentifier data");
                    throw ErrorCodes::Compression::ES17;
                }
                t.fileIdentifier = data.substr(pos, idLen);
                pos += idLen;
            }

            t.checksum = readValue<uint32_t>(data.data(), pos, totalSize);
            tokens.push_back(std::move(t));

        }

        uint32_t parsedGlobal = readValue<uint32_t>(data.data(), pos, totalSize);
        uint32_t sum = 0;
        for (auto &tk : tokens) sum += tk.checksum;
        if (sum != parsedGlobal) {
            ErrorHandler::logError("deserializeTokens", ErrorCodes::Compression::ES8,
                "Global checksum mismatch");
            throw ErrorCodes::Compression::ES8;
        }

        ErrorHandler::logInfo("LZ77::deserializeTokens", ErrorCodes::Compression::IC6,
            "deserializeTokens complete");
        return tokens;
    }


    CompressionResult compressBlob(const std::string& input, const bool includeID, const bool offset32) {
        CompressionResult result;
        try {
            std::string &out = result.data;
            out.reserve(input.size() + 64);

            //Store unaltered input in case of faliure
            //TODO: Make this be returned/logged with errors
            std::string original = input;

            static uint32_t nextPackageID = 1;
            uint32_t packageID = nextPackageID++;

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
                Token t;
                if (includeID) {
                    t.fileIdentifier = std::to_string(packageID);
                }
                t.use32BitOffset = offset32;

                // Token generation
                if (bestMatchLength >= MIN_MATCH_LENGTH) {
                    std::cout << "Match found: Offset = " << bestMatchOffset
                              << ", Length = " << bestMatchLength << "\n";
                    t.offset          = bestMatchOffset;
                    t.length          = static_cast<uint32_t>(bestMatchLength);
                    t.type            = Token::TokenType::MATCH;
                    // Next symbol after the match
                    if (currentPosition + bestMatchLength < inputLength)
                        t.literal = std::string(1, input[currentPosition + bestMatchLength]);
                    else
                        t.literal.clear();

                    //Checksum
                    uint32_t cs = static_cast<uint32_t>(t.offset) + t.length;
                    for (unsigned char c : t.literal) cs += c;
                    t.checksum = cs;

                    tokens.push_back(std::move(t));

                    // Advance the currentPosition by the length of the match plus one (if nextSymbol is used).
                    // Adjust this based on your token scheme.
                    currentPosition += bestMatchLength + t.literal.size();
                    // If you include the next symbol as a separate literal, you might want to add it and then advance by 1.
                } else {
                    // No match found: output a literal token.
                    std::cout << "No match, output literal: " << input[currentPosition] << "\n";
                    t.offset = 0;
                    t.length = 0;
                    t.type   = Token::TokenType::LITERAL;
                    t.literal = std::string(1, input[currentPosition]);
                    uint32_t cs = static_cast<uint8_t>(input[currentPosition]);
                    t.checksum = cs;
                    tokens.push_back(std::move(t));
                    currentPosition += 1;
                }
            }

            if (tokens.empty()) {
                ErrorHandler::logError("CompressionAPI::compressBlob", ErrorCodes::Compression::ES1,
                    "Compression failed: no tokens generated.");
                throw ErrorCodes::Compression::ES1;
            }
            uint32_t globalChecksum = 0;
            for (auto &tk : tokens) {
                globalChecksum += tk.checksum;
            }

            ErrorHandler::logInfo("CompressionAPI::compressBlob", ErrorCodes::Compression::IC7,
        "Global checksum = " + std::to_string(globalChecksum));

            result.data = serializeTokens(tokens, includeID, offset32, packageID, globalChecksum);

            //TODO: Proper check. For example checksum check?
            if (!result.data.empty()) {
                result.errorCode = ErrorCodes::Compression::SUCCESS;
            } else {
                ErrorHandler::logError("CompressionAPI::compressBlob", ErrorCodes::Compression::ES1,
                                         "Compression failed: Empty output generated.");
                result.errorCode = ErrorCodes::Compression::ES1;
            }
        } catch (ErrorCodes::Compression code) {
            result.data = ""; // Or original?
            result.errorCode = code;
        }
        return result;
    }

    CompressionResult decompressBlob(const std::string& input) {

        // TODO: Implement LZ77 decompression algorithm.
        // Steps to follow:
        // 1. Parse the stream of tokens (offset, length, literal) produced during compression.
        // 2. Reconstruct the original data using a sliding window.
        // 3. Log errors using ErrorHandler::logError if data integrity is compromised.

        CompressionResult result;
        try {
            auto tokens = deserializeTokens(input);
            std::string output;
            output.reserve(/*estimate*/);

            for (const auto &t : tokens) {
                switch (t.type) {
                    case Token::TokenType::LITERAL:
                        output += t.literal;
                    break;
                    case Token::TokenType::MATCH: {
                        // Offset must not exceed current output size
                        if (t.offset == 0 || t.offset > output.size()) {
                            throw ErrorCodes::Compression::ES10; // Invalid offset
                        }
                        size_t start = output.size() - t.offset;
                        // Copy match bytes
                        for (uint32_t i = 0; i < t.length; ++i) {
                            output += output[start + i];
                        }
                        // Append next literal
                        output += t.literal;
                        break;
                    }
                    case Token::TokenType::END_OF_BLOCK:
                        // Optionally break early
                            goto done;
                    default:
                        throw ErrorCodes::Compression::ES12; // Unsupported token
                }
            }
            done:
                result.data = std::move(output);
            result.errorCode = ErrorCodes::Compression::SUCCESS;
        } catch (ErrorCodes::Compression code) {
            ErrorHandler::logError("decompressBlob", code, "LZ77 decompression failed");
            result.data = ""; // Or original?
            result.errorCode = code;
        }
        return result;
    }

}
