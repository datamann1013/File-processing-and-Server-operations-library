// Created by data0 on 12.04.2025.

#ifndef ERRORCODES_H
#define ERRORCODES_H

namespace ErrorCodes {
    // Enumeration for compression-related error codes following our standard.
    // Format: [E/W][s/u/n][c][number]
    // E.g., ES1: Error, Server-side, Compression, error #1.
    enum class Compression {
        SUCCESS = 0,
        ES1, // e s c 1: Compression failure (server-side error).
        ES2, // e s c 2: Checksum mismatch (server-side error).
        EU1, // e u c 1: Invalid JSON input or permission issue (user-side error).
        // Add additional error codes as needed.
        // Warnings could be, e.g., WU1: Warning, user-side, compression.
        WU1,
        WS1,
        // Informational codes are handled separately, e.g., IC1: Info, compression.
        IC1
    };

    // Optional: A helper function to convert an error code into its string representation.
    // This function is useful for logging.
    inline std::string toString(Compression code) {
        switch (code) {
            case Compression::SUCCESS: return "SUCCESS";
            case Compression::ES1: return "ES1"; // Compression failure.
            case Compression::ES2: return "ES2"; // Checksum mismatch.
            case Compression::EU1: return "EU1"; // Invalid JSON / Permission denied.
            case Compression::WU1: return "WU1"; // User warning.
            case Compression::WS1: return "WS1"; // Server warning.
            case Compression::IC1: return "IC1"; // Informational.
            default: return "Unknown";
        }
    }
}

#endif //ERRORCODES_H
