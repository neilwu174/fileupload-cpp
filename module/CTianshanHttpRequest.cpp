//
// Created by developer on 2025-12-18.
//

#include "CTianshanHttpRequest.h"

#include <iostream>
#include <unistd.h>

#include "CTianshanHttpController.h"

bool CTianshanHttpRequest::readHeaders(int fd, std::string &raw) {
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
    bool check = raw.find("\r\n\r\n") != std::string::npos;
    if (!check) {
        return false;
    }
    parseRequest(raw);
    return true;
}

bool CTianshanHttpRequest::accept(int incoming) {
    std::string raw;
    if (!this->readHeaders(incoming, raw)) {
        return false;
    }

    // Read body if Content-Length present
    std::string clh = this->getHeader("content-length");
    size_t contentLength = 0;
    if (!clh.empty()) {
        contentLength = static_cast<size_t>(std::strtoull(clh.c_str(), nullptr, 10));
    }
    // Already have some body bytes after header delimiter
    std::string bodyAlready;
    if (this->getHeaderEnd() + 4 < raw.size()) {
        bodyAlready = raw.substr(this->getHeaderEnd() + 4);
    }
    this->setBody(bodyAlready);
    std::cout << "main() -- httpRequest.body.size :: " << this->getBody().size() << std::endl;
    std::cout << "main() -- contentLength :: " << contentLength << std::endl;
    if (this->getBody().size() < contentLength) {
        std::string rest = readAll(incoming, contentLength - this->getBody().size());
        this->setBody(this->getBody() + rest);
    }
    return true;
}

bool CTianshanHttpRequest::parseRequest(const std::string &raw) {
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
        if (!(l >> this->method >> this->path >> this->version)) return false;
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
        this->headers[key] = value;
    }
    std::cout << "parseRequest(ended)------>" << std::endl;
    return true;
}

std::string CTianshanHttpRequest::getHeader(const std::string &key) {
    auto it = this->headers.find(toLower(key));
    if (it != this->headers.end()) return it->second;
    return {};
}

bool CTianshanHttpRequest::route(const char * method, const char * path) {
    return this->method == method && this->path == path;
}
