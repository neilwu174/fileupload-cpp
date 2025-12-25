//
// Created by developer on 2025-12-19.
//

#ifndef FILEUPLOAD_CTIANSHANHTTPCONTROLLER_H
#define FILEUPLOAD_CTIANSHANHTTPCONTROLLER_H
#include <sys/socket.h>

#include "CTianshanConfig.h"
#include "CTianshanHttpRequest.h"
#include "CTianshanHttpResponse.h"

static bool sendAll(int fd, const std::string &data) {
    size_t total = 0;
    while (total < data.size()) {
        ssize_t n = ::send(fd, data.data() + total, data.size() - total, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        total += static_cast<size_t>(n);
    }
    return true;
}

static std::string readAll(int fd, size_t expected) {
    std::string out;
    out.reserve(expected);
    while (out.size() < expected) {
        char buf[8192];
        size_t toRead = std::min(expected - out.size(), sizeof(buf));
        ssize_t n = ::recv(fd, buf, toRead, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            break;
        } else if (n == 0) {
            break; // peer closed
        }
        out.append(buf, buf + n);
    }
    return out;
}

class CTianshanHttpController {
private:
    CTianshanConfig& config;
    std::map<std::string,std::function<CTianshanHttpResponse(CTianshanHttpRequest&)>> routes;
    std::function<CTianshanHttpResponse(CTianshanHttpRequest&)> getHandler(const char * method, const char * path) {
        return routes[std::string(method) + "-" + std::string(path)];
    }
public:
    CTianshanHttpController(CTianshanConfig& config):config(config){};
    void proceed(int incoming);
    void route(const char * method, const char * path,std::function<CTianshanHttpResponse(CTianshanHttpRequest&)> httpHandler);
};

#endif //FILEUPLOAD_CTIANSHANHTTPCONTROLLER_H
