//
// Created by developer on 2025-12-18.
//

#ifndef FILEUPLOAD_CTIANSHANHTTP_H
#define FILEUPLOAD_CTIANSHANHTTP_H
#include <map>
#include <string>
#include <sys/socket.h>
#include <sstream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class CTianshanHttpResponseHandler {
public:
    std::string makeResponse(int status, const std::string &statusText, const std::string &contentType, const std::string &body, const std::map<std::string,std::string> &extraHeaders = {});
private:
    size_t maxBytes = 64 * 1024;
};


#endif //FILEUPLOAD_CTIANSHANHTTP_H