// Created by data0 on 12.04.2025.

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>

#include "ErrorCodes.h"

// Centralizes error/info logging; retains last 100 info messages.
class ErrorHandler {
public:
    // Logs an error for the given module.
    // module: A string identifying the module (e.g., "CompressionAPI").
    // errorCode: The error code from ErrorCodes.
    // context: Additional contextual information.
    static void logError(const std::string& module, ErrorCodes::Compression errorCode, const std::string& context);

    // Logs an info for the given module.
    // Logs an info message; keeps last 100 entries in memory.
    static void logInfo(const std::string &module, ErrorCodes::Compression infoCode, const std::string &
                        context);

    // Retrieve the in-memory info log (most recent last), up to 100 entries.
    // Most recent entries appear at the end of the vector.
    static std::vector<std::string> getInfoLog();

private:
    // Turn an enum value into its literal name
    static const char* toString(ErrorCodes::Compression code);
};

#endif // ERROR_HANDLER_H
