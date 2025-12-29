//
// Created by developer on 2025-12-27.
//

#ifndef FILEUPLOAD_CTIANSHANFILESYSTEM_H
#define FILEUPLOAD_CTIANSHANFILESYSTEM_H
#include <string>
#include <vector>
#include "models.h"


class CTianshanFilesystem {
public:
    std::vector<TianshanFile> scan(std::string& path);
    // static bool downloadFile(const std::string& url, const std::string& localPath);
    bool downloadFile(const std::string& url, const std::string& localPath);
};


#endif //FILEUPLOAD_CTIANSHANFILESYSTEM_H