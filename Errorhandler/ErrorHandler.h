// Created by data0 on 12.04.2025.

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <string>
#include "ErrorCodes.h"

class ErrorHandler {
public:
    // Logs an error for a given module.
    // module: Identifier for the component (e.g., "CompressionAPI")
    // errorCode: The error code (as integer) from ErrorCodes.
    // context: A string describing the error context.
    static void logError(const std::string& module, int errorCode, const std::string& context);
};

#endif //ERRORHANDLER_H
