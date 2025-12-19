//
// Created by developer on 2025-12-18.
//

#include "CTianshanHttpResponseHandler.h"

#include <string>
#include <sstream>

std::string CTianshanHttpResponseHandler::makeResponse(int status, const std::string &statusText, const std::string &contentType, const std::string &body, const std::map<std::string,std::string> &extraHeaders) {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << ' ' << statusText << "\r\n";
    oss << "Date: " << httpDate() << "\r\n";
    oss << "Server: UploadService/1.0\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";
    for (const auto &kv : extraHeaders) {
        oss << kv.first << ": " << kv.second << "\r\n";
    }
    oss << "\r\n";
    oss << body;
    return oss.str();
}
