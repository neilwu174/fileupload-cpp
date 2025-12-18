//
// Created by developer on 2025-12-18.
//

#include "CTianshanHttp.h"

#include <sys/socket.h>
#include <iostream>
#include <string>
#include <sstream>

bool CTianshanHttp::parseRequest(const std::string &raw, HttpRequest &req, size_t &headerEnd) {
    std::cout << "parseRequest(started)------>" << std::endl;
    headerEnd = raw.find("\r\n\r\n");
    std::cout << "parseRequest() headerEnd=" << headerEnd << std::endl;
    if (headerEnd == std::string::npos) return false;
    std::string head = raw.substr(0, headerEnd);

    std::cout << "parseRequest() head=" << head << std::endl;

    std::istringstream iss(head);
    std::string line;
    if (!std::getline(iss, line)) return false;
    if (!line.empty() && line.back() == '\r') line.pop_back();
    {
        std::istringstream l(line);
        if (!(l >> req.method >> req.path >> req.version)) return false;
    }
    std::cout << "parseRequest() loop=" << std::endl;
    while (std::getline(iss, line)) {
        std::cout << "parseRequest(while-loop) line=" << line << std::endl;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto pos = line.find(':');
        if (pos == std::string::npos) continue;
        std::string key = toLower(line.substr(0, pos));
        std::string value = line.substr(pos + 1);
        // trim leading spaces
        size_t start = value.find_first_not_of(" \t");
        if (start != std::string::npos) value.erase(0, start);
        // trim trailing spaces
        while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) value.pop_back();
        req.headers[key] = value;
    }
    std::cout << "parseRequest(ended)------>" << std::endl;
    return true;
}

bool CTianshanHttp::readHeaders(int fd, std::string &raw) {
    raw.clear();
    while (raw.find("\r\n\r\n") == std::string::npos) {
        char tmp[2048];
        ssize_t n = ::recv(fd, tmp, sizeof(tmp), 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        if (n == 0) break; // connection closed
        raw.append(tmp, tmp + n);
        if (raw.size() > maxBytes) return false; // too large
    }
    return raw.find("\r\n\r\n") != std::string::npos;
}

std::string CTianshanHttp::makeResponse(int status, const std::string &statusText, const std::string &contentType, const std::string &body, const std::map<std::string,std::string> &extraHeaders) {
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
