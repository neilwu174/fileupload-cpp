//
// Created by developer on 2025-12-18.
//

#ifndef FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H
#define FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H
#include <filesystem>
#include <string>

#include "CTianshanHttpRequest.h"


class CTianshanMultipartHandler {

private:
    std::filesystem::path uploadDir;
    size_t bytesSaved;

public:
    CTianshanMultipartHandler(std::string uploadFolder) : uploadDir(uploadFolder) {}
    std::string extractParam(const std::string &headerValue, const std::string &param);
    bool handle(CTianshanHttpRequest& request, std::filesystem::path &savedPath, size_t &bytesSaved);
    bool saveToFile(const std::string &filename, const std::string &data, std::filesystem::path &outPath);
};


#endif //FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H