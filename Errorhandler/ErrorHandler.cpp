// Created by data0 on 12.04.2025.

#include "ErrorHandler.h"
#include <iostream>

void ErrorHandler::logError(const std::string& module, ErrorCodes::Compression errorCode, const std::string& context) {
    // Log the error to standard error. In a production system, this might log to a file or other system.
    std::cerr << "[" << module << "] Error Code: " << ErrorCodes::toString(errorCode)
              << " | Context: " << context << std::endl;
}