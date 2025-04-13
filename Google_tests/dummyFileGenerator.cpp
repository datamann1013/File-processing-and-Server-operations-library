// Created by data0 on 13.04.2025.

#include "dummyFileGenerator.h"

std::string generateDummyFileData(size_t sizeBytes) {
    std::string data;
    data.reserve(sizeBytes);
    const char pattern[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t patternSize = sizeof(pattern) - 1;
    for (size_t i = 0; i < sizeBytes; i++) {
        data.push_back(pattern[i % patternSize]);
    }
    return data;
}
