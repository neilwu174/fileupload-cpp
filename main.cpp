#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <cerrno>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>
#include <chrono>

#include "module/CTianshanHttp.h"
#include "module/CTianshanHttpRequest.h"
#include "module/CTianshanMultipartHandler.h"
#include "module/CUploadService.h"

namespace fs = std::filesystem;

static volatile std::sig_atomic_t g_shouldStop = 0;
static int g_listenFd = -1;

HttpConfig yaml;

void handleSigInt(int) {
    g_shouldStop = 1;
    if (g_listenFd >= 0) {
        ::close(g_listenFd);
    }
}

int main(int argc, const char * argv[]) {
    std::signal(SIGINT, handleSigInt);
    CUploadService upload_service;
    CTianshanHttp tianshan_http;

    upload_service.load_config(yaml);
    int port = yaml.port;
    g_listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (g_listenFd < 0) {
        std::cerr << "Failed to create socket: " << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }

    int yes = 1;
    ::setsockopt(g_listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (::bind(g_listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Bind failed: " << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }

    if (::listen(g_listenFd, 16) < 0) {
        std::cerr << "Listen failed: " << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }

    std::cerr << "HTTP server listening on http://0.0.0.0:" << port << "\n";

    while (!g_shouldStop) {
        sockaddr_in cli{}; socklen_t clilen = sizeof(cli);
        int cfd = ::accept(g_listenFd, reinterpret_cast<sockaddr*>(&cli), &clilen);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) { continue; }
            // transient error
            continue;
        }

        // Read headers
        std::string raw;
        CTianshanHttpRequest req;
        if (!req.readHeaders(cfd, raw)) {
            std::string resp = tianshan_http.makeResponse(400, "Bad Request", "text/plain; charset=utf-8", "Malformed headers\n");
            sendAll(cfd, resp);
            ::close(cfd);
            continue;
        }

        // Read body if Content-Length present
        std::string clh = req.getHeader("content-length");
        size_t contentLength = 0;
        if (!clh.empty()) {
            contentLength = static_cast<size_t>(std::strtoull(clh.c_str(), nullptr, 10));
        }
        // Already have some body bytes after header delimiter
        std::string bodyAlready;
        if (req.getHeaderEnd() + 4 < raw.size()) {
            bodyAlready = raw.substr(req.getHeaderEnd() + 4);
        }
        req.setBody(bodyAlready);
        std::cout << "main() -- req.body.size :: " << req.getBody().size() << std::endl;
        std::cout << "main() -- contentLength :: " << contentLength << std::endl;
        if (req.getBody().size() < contentLength) {
            std::string rest = readAll(cfd, contentLength - req.getBody().size());
            req.setBody(req.getBody() + rest);
        }

        std::string transferEnc = toLower(req.getHeader("transfer-encoding"));

        std::cout << "main() transferEnc=" << transferEnc << std::endl;

        if (!transferEnc.empty() && transferEnc.find("chunked") != std::string::npos) {
            std::string resp = tianshan_http.makeResponse(501, "Not Implemented", "application/json", "{\n  \"ok\": false, \"error\": \"chunked transfer not supported\"\n}\n");
            sendAll(cfd, resp);
            ::close(cfd);
            continue;
        }

        std::string response;
        if (req.getMethod() == "GET" && req.getPath() == "/") {
            std::string html =
                "<!doctype html>\n<html><head><meta charset=\"utf-8\"><title>Upload</title></head>"
                "<body><h1>Upload</h1>"
                "<form method=\"POST\" action=\"/upload\" enctype=\"multipart/form-data\">"
                "<input type=\"file\" name=\"file\" />"
                "<button type=\"submit\">Upload</button>"
                "</form></body></html>";
            response = tianshan_http.makeResponse(200, "OK", "text/html; charset=utf-8", html);
        } else if (req.getMethod() == "POST" && req.getPath() == "/upload") {
            fs::path saved;
            size_t bytes = 0;
//            std::string ctype = toLower(getHeader(req, "content-type"));
            std::string ctype = req.getHeader("content-type");
            bool ok = false;
            if (ctype.find("multipart/form-data") != std::string::npos) {
                CTianshanMultipartHandler handler(yaml.uploadFolder);
                ok = handler.handle(req, saved, bytes);
                if (!ok) {
                    std::string body = "{\n  \"ok\": false, \"error\": \"invalid multipart form data\"\n}\n";
                    response = tianshan_http.makeResponse(400, "Bad Request", "application/json", body);
                }
            }
            if (ok) {
                std::ostringstream json;
                json << "{\n  \"ok\": true, \"filename\": \"" << saved.string() << "\", \"bytes\": " << bytes << "\n}\n";
                response = tianshan_http.makeResponse(200, "OK", "application/json", json.str());
                std::cerr << "Saved file: " << saved << " (" << bytes << " bytes)\n";
            }
        } else if (req.getMethod() == "GET" || req.getMethod() == "POST" || req.getMethod() == "HEAD" || req.getMethod() == "PUT" || req.getMethod() == "DELETE") {
            response = tianshan_http.makeResponse(404, "Not Found", "text/plain; charset=utf-8", "Not Found\n");
        } else {
            response = tianshan_http.makeResponse(405, "Method Not Allowed", "text/plain; charset=utf-8", "Method Not Allowed\n", {{"Allow", "GET, POST"}});
        }

        sendAll(cfd, response);
        ::close(cfd);
    }

    std::cerr << "Shutting down.\n";
    return EXIT_SUCCESS;
}
