//
// Created by developer on 2025-12-19.
//

#ifndef FILEUPLOAD_CTIANSHANHTTPCONTROLLER_H
#define FILEUPLOAD_CTIANSHANHTTPCONTROLLER_H
#include <sys/socket.h>

#include "CTianshanConfig.h"

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

public:
    void accept(int incoming,CTianshanConfig& config);
};

#endif //FILEUPLOAD_CTIANSHANHTTPCONTROLLER_H
