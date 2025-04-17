// Created by data0 on 12.04.2025.

#include "ErrorHandler.h"
#include <iostream>

void ErrorHandler::logError(const std::string& module, ErrorCodes::Compression errorCode, const std::string& context) {
    // Log standard error stream.
    // Will log to database later
    std::cerr << "[" << module << "] Error Code: " << static_cast<int>(errorCode)
              << " | Context: " << context << std::endl;
}

void ErrorHandler::logInfo(const std::string& module, ErrorCodes::Compression errorCode, const std::string& context) {
    // Log standard error stream.
    // Will log to database later
    std::cerr << "[" << module << "] Error Code: " << static_cast<int>(errorCode)
              << " | Context: " << context << std::endl;
}