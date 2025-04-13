// Created by data0 on 13.04.2025.

#include "LZMACompressor.h"
#include "../../../errorhandler/ErrorHandler.h"

CompressionAPI::CompressionResult LZMACompressor::compress(const std::string &inputData) {
    CompressionAPI::CompressionResult result;
    // Future implementation: Apply LZMA compression on inputData.
    ErrorHandler::logError("LZMACompressor::compress", ErrorCodes::Compression::ENN99, "LZMA compress not implemented.");
    result.errorCode = ErrorCodes::Compression::ENN99;
    result.data = "";
    return result;
}

CompressionAPI::CompressionResult LZMACompressor::decompress(const std::string &inputData) {
    CompressionAPI::CompressionResult result;
    // Future implementation: Apply LZMA decompression on inputData.
    ErrorHandler::logError("LZMACompressor::decompress", ErrorCodes::Compression::ENN99, "LZMA decompress not implemented.");
    result.errorCode = ErrorCodes::Compression::ENN99;
    result.data = "";
    return result;
}
