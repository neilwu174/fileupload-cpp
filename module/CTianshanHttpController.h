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
    std::string createContentType(const std::string &filename) {
        if (filename.find(".html") != std::string::npos) return "text/html; charset=utf-8";
        if (filename.find(".css") != std::string::npos) return "text/css; charset=utf-8";
        if (filename.find(".js") != std::string::npos) return "application/javascript; charset=utf-8";
        if (filename.find(".png") != std::string::npos) return "image/png";
        if (filename.find(".jpg") != std::string::npos) return "image/jpeg";
        if (filename.find(".gif") != std::string::npos) return "image/gif";
        if (filename.find(".ico") != std::string::npos) return "image/x-icon";
        if (filename.find(".svg") != std::string::npos) return "image/svg+xml";
        if (filename.find(".ttf") != std::string::npos) return "application/x-font-ttf";
        if (filename.find(".woff") != std::string::npos) return "application/font-woff";
        if (filename.find(".woff2") != std::string::npos) return "application/font-woff2";
        if (filename.find(".eot") != std::string::npos) return "application/vnd.ms-fontobject";
        if (filename.find(".otf") != std::string::npos) return "application/font-sfnt";
        if (filename.find(".zip") != std::string::npos) return "application/zip";
        if (filename.find(".rar") != std::string::npos) return "application/x-rar-compressed";
        if (filename.find(".pdf") != std::string::npos) return "application/pdf";
        if (filename.find(".txt") != std::string::npos) return "text/plain; charset=utf-8";
        if (filename.find(".xml") != std::string::npos) return "text/xml; charset=utf-8";
        if (filename.find(".json") != std::string::npos) return "application/json; charset=utf-8";
        if (filename.find(".csv") != std::string::npos) return "text/csv; charset=utf-8";
        if (filename.find(".mp3") != std::string::npos) return "audio/mpeg";
        if (filename.find(".wav") != std::string::npos) return "audio/wav";
        if (filename.find(".mp4") != std::string::npos) return "video/mp4";
        if (filename.find(".avi") != std::string::npos) return "video/avi";
        if (filename.find(".mov") != std::string::npos) return "video/quicktime";
        if (filename.find(".mpeg") != std::string::npos) return "video/mpeg";
        if (filename.find(".3gp") != std::string::npos) return "video/3gpp";
        if (filename.find(".flv") != std::string::npos) return "video/x-flv";
        if (filename.find(".swf") != std::string::npos) return "application/x-shockwave-flash";
        if (filename.find(".exe") != std::string::npos) return "application/octet-stream";
        if (filename.find(".msi") != std::string::npos) return "application/octet-stream";
        return "application/octet-stream";
    }
};

#endif //FILEUPLOAD_CTIANSHANHTTPCONTROLLER_H
