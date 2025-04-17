// Created by data0 on 12.04.2025.

#include "ErrorHandler.h"

#include <deque>
#include <iostream>
#include <mutex>
#include <sstream>

void ErrorHandler::logError(const std::string& module, ErrorCodes::Compression errorCode, const std::string& context) {
    // Log standard error stream.
    // Will log to database later
    std::cerr << "[" << module << "] Error Code: " << static_cast<int>(errorCode)
              << " | Context: " << context << std::endl;
}

// Static buffer + mutex for thread-safety
static std::deque<std::string> _infoBuffer;
static std::mutex      _infoMutex;

void ErrorHandler::logInfo(const std::string& module, ErrorCodes::Compression infoCode, const std::string& context) {

    // 1) Emit to the standard info stream
    std::clog << "[" << module << "] Info Code: " << static_cast<int>(infoCode) << " | Context: " << context << std::endl;

    // 2) Store in our fixed‑size buffer
    std::lock_guard<std::mutex> lk(_infoMutex);
    std::ostringstream oss;
    oss << "[" << module << "] Info Code: " << static_cast<int>(infoCode) << " | Context: " << context;
    _infoBuffer.push_back(oss.str());
    if (_infoBuffer.size() > 100) {
        _infoBuffer.pop_front();
    }
}

std::vector<std::string> ErrorHandler::getInfoLog() {
    std::lock_guard lk(_infoMutex);
    return { _infoBuffer.begin(), _infoBuffer.end() };
}