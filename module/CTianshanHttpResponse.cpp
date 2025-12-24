//
// Created by developer on 2025-12-21.
//
#include <sstream>
#include "CTianshanHttpResponse.h"

static inline std::string httpDate() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    char buf[128] = {0};
    std::tm tm{};
#if defined(__APPLE__)
    gmtime_r(&t, &tm);
#else
    tm = *std::gmtime(&t);
#endif
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &tm);
    return std::string(buf);
}

std::string CTianshanHttpResponse::build() {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << ' ' << statusText << "\r\n";
    oss << "Date: " << httpDate() << "\r\n";
    oss << "Server: " << this->agent << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";
    for (const auto &kv : this->headers) {
        oss << kv.first << ": " << kv.second << "\r\n";
    }
    oss << "\r\n";
    oss << body;
    return oss.str();
}
