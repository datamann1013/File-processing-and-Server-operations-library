// Created by data0 on 13.04.2025.

#include "ZIPCompressor.h"
#include "../../../errorhandler/ErrorHandler.h"

CompressionAPI::CompressionResult ZIPCompressor::compress(const std::string &inputData) {
    CompressionAPI::CompressionResult result;
    // Future implementation: Apply ZIP compression.
    ErrorHandler::logError("ZIPCompressor::compress", ErrorCodes::Compression::ENN99, "ZIP compress not implemented.");
    result.errorCode = ErrorCodes::Compression::ENN99;
    result.data = "";
    return result;
}

CompressionAPI::CompressionResult ZIPCompressor::decompress(const std::string &inputData) {
    CompressionAPI::CompressionResult result;
    // Future implementation: Apply ZIP decompression.
    ErrorHandler::logError("ZIPCompressor::decompress", ErrorCodes::Compression::ENN99, "ZIP decompress not implemented.");
    result.errorCode = ErrorCodes::Compression::ENN99;
    result.data = "";
    return result;
}
