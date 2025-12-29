//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_FILE_UTILS_H
#define FILEUPLOAD_FILE_UTILS_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>

#include "models.h"

namespace fs = std::filesystem;

// Function to read the entire content of a file into a string
inline std::string readFileContent(const fs::path& filePath) {
    std::cout << "Reading file: " << filePath << std::endl;
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return "";
    }

    // Read the entire file into a stringstream and then to a string
    std::ostringstream contentStream;
    contentStream << fileStream.rdbuf();
    fileStream.close(); // Good practice to close the file
    return contentStream.str();
}

/**
 * Parses a template include tag and extracts the filename.
 * Example: {% include "header.html" %} -> header.html
 */
inline std::string extractIncludePath(const std::string& input) {
    std::regex include_regex("\\{%\\s*include\\s*\"([^\"]+)\"\\s*%\\}");
    std::smatch match;
    if (std::regex_search(input, match, include_regex)) {
        if (match.size() > 1) {
            return match.str(1);
        }
    }
    return "";
}

/**
 * Reads a file line by line and returns a vector of strings.
 */
inline std::vector<std::string> readFileLines(const fs::path& filePath) {
    std::vector<std::string> lines;
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(fileStream, line)) {
        std::string filename = extractIncludePath(line);
        if (!filename.empty()) {
            std::cout << "Found include: " << filename << std::endl;
            std::string includeContent = readFileContent(filePath.parent_path() / filename);
            std::cout << "Replacing include with content: " << includeContent << std::endl;
            lines.push_back(includeContent);
        } else {
            lines.push_back(line);
        }
    }
    fileStream.close();
    return lines;
}

/**
 * Joins a vector of strings into a single string with a delimiter.
 */
inline std::string join(const std::vector<std::string>& elements, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < elements.size(); ++i) {
        oss << elements[i];
        if (i < elements.size() - 1) {
            oss << delimiter;
        }
    }
    return oss.str();
}

inline std::string readFileAsString(const fs::path& filePath) {
    std::vector<std::string> lines = readFileLines(filePath);
    return join(lines, "\n");
}

inline std::vector<TianshanFile> scan_directory(std::string &path) {
    std::cout << "scan_directory-started" << std::endl;
    std::vector<TianshanFile> files;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::cout << "Path:" << entry.path() << std::endl;
        std::cout << "   Parent:" << entry.path().parent_path() << std::endl;
        std::cout << "   Name:" << entry.path().filename() << std::endl;
        TianshanFile file;
        file.name = entry.path().filename().string();
        file.path = entry.path().string();
        file.parent = entry.path().parent_path().string();
        try {
            if (!entry.is_directory()) {
                std::cout << "   Size:" << entry.file_size() << std::endl;
                file.size = entry.file_size();
                file.kind = "file";
                file.image = "file.png";
            } else {
                file.kind = "directory";
                file.image = "folder.png";
            }
            files.push_back(file);
        } catch (const std::exception& e) {
            std::cerr << "General exception caught: " << e.what() << std::endl;
        }
    };
    return files;
}

#endif //FILEUPLOAD_FILE_UTILS_H