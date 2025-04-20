// Created by data0 on 12.04.2025.

#include "ErrorHandler.h"

#include <deque>
#include <iostream>
#include <mutex>
#include <sstream>

using ErrorCodes::Compression;

//Store last 100 info messages
static std::deque<std::string> _infoBuffer;
static std::mutex              _infoMutex;


const char* ErrorHandler::toString(Compression code) {
    switch (code) {
        case Compression::SUCCESS:      return "SUCCESS";
        case Compression::ES1:          return "ES1";
        case Compression::ES2:          return "ES2";
        case Compression::ES3:          return "ES3";
        case Compression::ES4:          return "ES4";
        case Compression::ES5:          return "ES5";
        case Compression::ES6:          return "ES6";
        case Compression::ES7:          return "ES7";
        case Compression::ES8:          return "ES8";
        case Compression::ES9:          return "ES9";
        case Compression::ES10:         return "ES10";
        case Compression::ES11:         return "ES11";
        case Compression::ES12:         return "ES12";
        case Compression::ES13:         return "ES13";
        case Compression::ES14:         return "ES14";
        case Compression::ES15:         return "ES15";
        case Compression::ES16:         return "ES16";
        case Compression::ES17:         return "ES17";

        case Compression::EU1:          return "EU1";
        case Compression::EU2:          return "EU2";
        case Compression::EU3:          return "EU3";

        case Compression::WU1:          return "WU1";
        case Compression::WU2:          return "WU2";
        case Compression::WS1:          return "WS1";

        case Compression::IC1:          return "IC1";
        case Compression::IC2:          return "IC2";
        case Compression::IC3:          return "IC3";
        case Compression::IC4:          return "IC4";
        case Compression::IC5:          return "IC5";
        case Compression::IC6:          return "IC6";

        case Compression::ENN99:        return "ENN99";
        default:                        return "UNKNOWN_CODE";
    }
}


void ErrorHandler::logError(const std::string& module, Compression errorCode, const std::string& context) {
    // Log standard error stream.
    // Will log to database later
    std::cerr << "[" << module << "] Error Code: " << toString(errorCode) << " / " << static_cast<int>(errorCode)
              << " | Context: " << context << std::endl;
}

void ErrorHandler::logInfo(const std::string& module, ErrorCodes::Compression infoCode, const std::string& context) {

    // 1) Emit to the standard info stream
    std::clog << "[" << module << "] Info Code: " << toString(infoCode) << " / " << static_cast<int>(infoCode) << " | Context: " << context << std::endl;

    // 2) Store in our fixed‑size buffer
    std::lock_guard<std::mutex> lk(_infoMutex);
    std::ostringstream oss;
    oss << "[" << module << "] Info Code: " << toString(infoCode) << " / " << static_cast<int>(infoCode) << " | Context: " << context;
    _infoBuffer.push_back(oss.str());
    if (_infoBuffer.size() > 100) {
        _infoBuffer.pop_front();
    }
}

std::vector<std::string> ErrorHandler::getInfoLog() {
    std::lock_guard lock(_infoMutex);
    return { _infoBuffer.begin(), _infoBuffer.end() };
}