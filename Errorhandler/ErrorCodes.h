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

        ES1, //Compression failure.
        ES2, //Checksum mismatch.
        ES3, //Decompression failure.
        ES4, //Data integrity error.

        EU1, //Invalid JSON input.
        EU2, //Unsupported file type.
        EU3, //Permission denied.

        WU1, //Missing or invalid precompression parameters.
        WU2, //Encryption flag is set but not implemented.
        WS1, //Low compression ratio or performance issue.

        IC1,  //Operation initiated.

        ENN99 // Not implemented
    };

}

#endif // ERROR_CODES_H
