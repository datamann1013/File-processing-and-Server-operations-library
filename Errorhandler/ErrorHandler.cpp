// Created by data0 on 12.04.2025.

#include "ErrorHandler.h"
#include <iostream>

void ErrorHandler::logError(const std::string& module, int errorCode, const std::string& context) {
    // For now, simply print the error to standard error.
    std::cerr << "[" << module << "] Error Code: " << errorCode
              << " | Context: " << context << std::endl;
}