//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_FILE_UTILS_H
#define FILEUPLOAD_FILE_UTILS_H

#include <filesystem>
namespace fs = std::filesystem;

// Function to read the entire content of a file into a string
inline std::string readFileContent(const fs::path& filePath) {
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

#endif //FILEUPLOAD_FILE_UTILS_H