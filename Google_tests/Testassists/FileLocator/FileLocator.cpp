// Created by admin on 14.04.2025.

#include "FileLocator.h"

namespace FileCompression {
    namespace fs = std::filesystem;

    std::vector<fs::path> FileLocator::getFilesByType(const std::string &directory, const std::string &typeFilter) {
        std::vector<fs::path> files;
        // Define acceptable extensions for images and videos.
        const std::vector<std::string> imageExtensions = {".jpg", ".jpeg", ".png", ".bmp", ".gif"};
        const std::vector<std::string> videoExtensions = {".mp4", ".avi", ".mkv", ".mov"};

        for (const auto &entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                fs::path p = entry.path();
                if (typeFilter == "null") {
                    files.push_back(p);
                } else if (typeFilter == "image") {
                    for (const auto &ext : imageExtensions) {
                        if (p.extension() == ext) {
                            files.push_back(p);
                            break;
                        }
                    }
                } else if (typeFilter == "video") {
                    for (const auto &ext : videoExtensions) {
                        if (p.extension() == ext) {
                            files.push_back(p);
                            break;
                        }
                    }
                }
            }
        }
        return files;
    }
}