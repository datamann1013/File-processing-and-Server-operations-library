// Created by data0 on 12.04.2025.

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

// Error codes centralized. Naming convention:
//  E: Error, W: Warning, I: Informational,
//  S: Server-side U: User-side N: Neither
//  then a letter for the component (c for compression),
//  then a numeric identifier.

namespace ErrorCodes {

    // Enumeration for compression/decompression error codes.
    enum class Compression {
        SUCCESS = 0,

        ES1,  // Compression failure.
        ES2,  // Checksum mismatch.
        ES3,  // Decompression failure.
        ES4,  // Data integrity error.

        ES5,  // Data too small to contain token count (LZ77)
        ES6,  // Data too short for literal (LZ77)
        ES7,  // Data too short for file identifier (LZ77)
        ES8,  // Data too short for checksum (LZ77)
        ES9,  // Not enough data to read value (LZ77)
        ES10, // Data too short for offset (LZ77)
        ES11, // Data too short for length (LZ77)
        ES12, // Data too short for token type (LZ77)
        ES13, // Data too short for literal length (LZ77)
        ES14, // Data too short for file identifier length (LZ77)
        ES15,  // Missing start header parameters.
        ES16, // Invalid start header format.
        ES17, //Turncated data (LZ77)

        EU1,  // Invalid JSON input.
        EU2,  // Unsupported file type.
        EU3,  // Permission denied.

        WU1,  // Missing or invalid precompression parameters.
        WU2,  // Encryption flag is set but not implemented.

        WS1,  // Low compression ratio or performance issue.

        IC1,  // Operation initiated
        IC2,  // Operation complete
        IC3,  // SerializeTokens start
        IC4,  // SerializeTokens complete
        IC5,  // DeserializeTokens start
        IC6,  // DeserializeTokens complete

        ENN99 // Not implemented
    };

}

#endif // ERROR_CODES_H
