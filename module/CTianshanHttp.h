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

static std::string generateFileName(const std::string &ext = "bin") {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "upload-" << ms << "." << ext;
    return oss.str();
}

static bool saveToFile(const fs::path &dir, const std::string &filename, const std::string &data, fs::path &outPath) {
    try {
        fs::create_directories(dir);
        outPath = dir / filename;
        std::ofstream ofs(outPath, std::ios::binary);
        if (!ofs) return false;
        ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
        return ofs.good();
    } catch (...) {
        return false;
    }
}

class CTianshanHttp {
public:
    std::string makeResponse(int status, const std::string &statusText, const std::string &contentType, const std::string &body, const std::map<std::string,std::string> &extraHeaders = {});
private:
    size_t maxBytes = 64 * 1024;
};


#endif //FILEUPLOAD_CTIANSHANHTTP_H