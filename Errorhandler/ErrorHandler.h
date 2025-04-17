// Created by data0 on 12.04.2025.

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>

#include "ErrorCodes.h"

// Centralizes error logging and management
class ErrorHandler {
public:
    // Logs an error for the given module.
    // module: A string identifying the module (e.g., "CompressionAPI").
    // errorCode: The error code from ErrorCodes.
    // context: Additional contextual information.
    static void logError(const std::string& module, ErrorCodes::Compression errorCode, const std::string& context);

    // Logs an info for the given module.
    // Logs an info message; keeps last 100 entries in memory.
    static void logInfo(const std::string &module, ErrorCodes::Compression infoCode, const std::string &context);

    // Retrieve the in-memory info log (most recent last), up to 100 entries.
    static std::vector<std::string> getInfoLog();
};

#endif // ERROR_HANDLER_H
