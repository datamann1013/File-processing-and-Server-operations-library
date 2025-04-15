// Created by admin on 14.04.2025.

#ifndef FILE_LOCATOR_H
#define FILE_LOCATOR_H

#include <string>
#include <vector>
#include <filesystem>

namespace FileCompression {
    namespace fs = std::filesystem;

    // The FileLocator class provides a static method to retrieve files
    // from a given directory based on a type filter.
    // typeFilter:
    //   - "null": returns all files,
    //   - "image": returns files with extensions .jpg, .jpeg, .png, .bmp, .gif,
    //   - "video": returns files with extensions .mp4, .avi, .mkv, .mov.
    class FileLocator {
    public:
        static std::vector<fs::path> getFilesByType(const std::string &directory, const std::string &typeFilter);
    };
}

#endif // FILE_LOCATOR_H