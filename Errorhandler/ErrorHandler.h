// Created by data0 on 12.04.2025.

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include "ErrorCodes.h"

// A centralized error handler for logging errors.
class ErrorHandler {
public:
    // Logs an error with a given module name, error code, and context information.
    static void logError(const std::string& module, ErrorCodes::Compression errorCode, const std::string& context);
};

#endif // ERROR_HANDLER_H