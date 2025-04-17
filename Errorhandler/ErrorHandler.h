// Created by data0 on 12.04.2025.

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
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
    static void logInfo(const std::string &module, ErrorCodes::Compression errorCode, const std::string &context);
};

#endif // ERROR_HANDLER_H
