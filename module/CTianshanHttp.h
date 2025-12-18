//
// Created by developer on 2025-12-18.
//

#ifndef FILEUPLOAD_CTIANSHANHTTP_H
#define FILEUPLOAD_CTIANSHANHTTP_H
#include <map>
#include <string>
#include <sys/socket.h>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string,std::string> headers; // lower-cased keys
    std::string body;
};
static inline std::string toLower(std::string s) {
    for (auto &c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

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

class CTianshanHttp {
public:
    bool parseRequest(const std::string &raw, HttpRequest &req, size_t &headerEnd);
    bool readHeaders(int fd, std::string &raw);
    std::string makeResponse(int status, const std::string &statusText, const std::string &contentType, const std::string &body, const std::map<std::string,std::string> &extraHeaders = {});

private:
    size_t maxBytes = 64 * 1024;
};


#endif //FILEUPLOAD_CTIANSHANHTTP_H