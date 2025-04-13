// Created by data0 on 13.04.2025.

#include "JPEGCompressor.h"
#include "../../../errorhandler/ErrorHandler.h"

CompressionAPI::CompressionResult JPEGCompressor::compress(const std::string &inputData) {
    CompressionAPI::CompressionResult result;
    // Future implementation: Apply JPEG image compression.
    ErrorHandler::logError("JPEGCompressor::compress", ErrorCodes::Compression::ENN99, "JPEG compress not implemented.");
    result.errorCode = ErrorCodes::Compression::ENN99;
    result.data = "";
    return result;
}

CompressionAPI::CompressionResult JPEGCompressor::decompress(const std::string &inputData) {
    CompressionAPI::CompressionResult result;
    // Future implementation: Apply JPEG image decompression.
    ErrorHandler::logError("JPEGCompressor::decompress", ErrorCodes::Compression::ENN99, "JPEG decompress not implemented.");
    result.errorCode = ErrorCodes::Compression::ENN99;
    result.data = "";
    return result;
}
