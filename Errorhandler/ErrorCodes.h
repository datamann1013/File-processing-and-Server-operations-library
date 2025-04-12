// Created by data0 on 12.04.2025.

#ifndef ERRORCODES_H
#define ERRORCODES_H

namespace ErrorCodes {
    // Enumeration for compression-related error codes.
    enum class Compression {
        SUCCESS = 0,
        INVALID_JSON = 1,             // JSON input is invalid.
        UNSUPPORTED_FILE_TYPE = 2,    // The file type is not supported.
        PERMISSION_DENIED = 3,        // Insufficient permissions.
        COMPRESSION_FAILED = 4,       // Compression process failed.
        CHECKSUM_MISMATCH = 5,        // Integrity verification failed.
        LOSSY_CONFIRMATION_NEEDED = 6 // Lossy compression confirmation required.
    };
}

#endif //ERRORCODES_H
