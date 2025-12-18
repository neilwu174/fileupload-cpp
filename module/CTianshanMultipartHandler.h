//
// Created by developer on 2025-12-18.
//

#ifndef FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H
#define FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H
#include <filesystem>
#include <string>


class CTianshanMultipartHandler {

private:
    std::filesystem::path uploadDir;
public:
    CTianshanMultipartHandler(std::string uploadFolder) : uploadDir(uploadFolder) {}
    std::string extractParam(const std::string &headerValue, const std::string &param);
    bool handle(const std::string &contentType, const std::string &body, std::filesystem::path &savedPath, size_t &bytesSaved);
    bool saveToFile(const std::string &filename, const std::string &data, std::filesystem::path &outPath);
};


#endif //FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H