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

std::string extractParam(const std::string &headerValue, const std::string &param) {
    // naive param extractor: param="value"
    std::string key = param + "=";
    auto pos = headerValue.find(key);
    if (pos == std::string::npos) return {};
    pos += key.size();
    if (pos < headerValue.size() && headerValue[pos] == '"') {
        auto end = headerValue.find('"', pos + 1);
        if (end != std::string::npos) return headerValue.substr(pos + 1, end - (pos + 1));
    } else {
        // token form until ; or end
        auto end = headerValue.find(';', pos);
        return headerValue.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
    }
    return {};
}

bool handleMultipart(const std::string &contentType, const std::string &body, fs::path &savedPath, size_t &bytesSaved) {
    std::cout << "handleMultipart(started)" << std::endl;
    std::cout << "handleMultipart(started)-contentType=" << contentType << std::endl;
    std::cout << "handleMultipart(started)-savedPath=" << savedPath << std::endl;
//    std::cout << "handleMultipart(started)-body=" << body << std::endl;

    auto bpos = contentType.find("boundary=");
    if (bpos == std::string::npos) return false;
    std::string boundary = contentType.substr(bpos + 9);
    if (!boundary.empty() && boundary.front() == '"' && boundary.back() == '"') {
        boundary = boundary.substr(1, boundary.size() - 2);
    }
    std::string sep = "--" + boundary;
    std::string endSep = sep + "--";

    std::cout << "handleMultipart(started)-sep=" << sep << std::endl;
    std::cout << "handleMultipart(started)-endSep=" << endSep << std::endl;

    // Split parts by boundary lines
    size_t pos = 0;
    bool foundFile = false;
    std::cout << "handleMultipart(loop)" << std::endl;
    while (true) {
        size_t start = body.find(sep, pos);
        std::cout << "handleMultiPart() start=" << start << std::endl;
        std::cout << "handleMultiPart() std::string::npos=" << std::string::npos << std::endl;

        if (start == std::string::npos) break;
        start += sep.size();
        if (start < body.size() && body[start] == '\r') start++;
        if (start < body.size() && body[start] == '\n') start++;
        size_t next = body.find(sep, start);
        if (next == std::string::npos) next = body.find(endSep, start);
        if (next == std::string::npos) break;
        size_t partEnd = next;
        // Trim trailing CRLF
        if (partEnd >= 1 && body[partEnd-1] == '\n') partEnd--;
        if (partEnd >= 1 && body[partEnd-1] == '\r') partEnd--;

        // part headers/body split
        size_t hEnd = body.find("\r\n\r\n", start);
        if (hEnd == std::string::npos || hEnd > partEnd) { pos = next; continue; }
        std::string pHeaders = body.substr(start, hEnd - start);
        std::string pBody = body.substr(hEnd + 4, partEnd - (hEnd + 4));

        std::cout << "handleMultiPart() pHeaders=" << pHeaders << std::endl;
//        std::cout << "handleMultiPart() pBody=" << pBody << std::endl;

        // parse headers
        std::istringstream ph(pHeaders);
        std::string hline;
        std::map<std::string,std::string> hmap;
        while (std::getline(ph, hline)) {

            std::cout << "handleMultiPart() hline=" << hline << std::endl;

            if (!hline.empty() && hline.back() == '\r') hline.pop_back();
            auto c = hline.find(':');
            if (c == std::string::npos) continue;
            std::string k = toLower(hline.substr(0, c));
            std::string v = hline.substr(c + 1);
            size_t s = v.find_first_not_of(" \t");
            if (s != std::string::npos) v.erase(0, s);
            while (!v.empty() && (v.back() == ' ' || v.back() == '\t')) v.pop_back();
            hmap[k] = v;
        }
        auto cdIt = hmap.find("content-disposition");
        if (cdIt != hmap.end()) {
            std::string cd = cdIt->second;
            std::string filename = extractParam(cd, "filename");
            if (!filename.empty()) {
                // sanitize filename: keep only basename
                try {
                    filename = fs::path(filename).filename().string();
                } catch (...) {
                    // fallback
                }
                if (filename.empty()) filename = generateFileName("bin");
                fs::path out;
                if (saveToFile(yaml.uploadFolder, CUploadService::get_filename(), pBody, out)) {
                    savedPath = out;
                    bytesSaved = pBody.size();
                    foundFile = true;
                    break;
                }
            }
        }
        pos = next;
    }
    return foundFile;
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
        if (!tianshan_http.readHeaders(cfd, raw)) {
            std::string resp = tianshan_http.makeResponse(400, "Bad Request", "text/plain; charset=utf-8", "Malformed headers\n");
            sendAll(cfd, resp);
            ::close(cfd);
            continue;
        }
        HttpRequest req;
        size_t headerEnd = 0;
        if (!tianshan_http.parseRequest(raw, req, headerEnd)) {
            std::string resp = tianshan_http.makeResponse(400, "Bad Request", "text/plain; charset=utf-8", "Cannot parse request\n");
            sendAll(cfd, resp);
            ::close(cfd);
            continue;
        }

        // Read body if Content-Length present
        std::string clh = getHeader(req, "content-length");
        size_t contentLength = 0;
        if (!clh.empty()) {
            contentLength = static_cast<size_t>(std::strtoull(clh.c_str(), nullptr, 10));
        }
        // Already have some body bytes after header delimiter
        std::cout << "main() -- headerEnd :: " << headerEnd << std::endl;
        std::cout << "main() -- raw.size :: " << raw.size() << std::endl;
        std::string bodyAlready;
        if (headerEnd + 4 < raw.size()) {
            bodyAlready = raw.substr(headerEnd + 4);
        }
        req.body = bodyAlready;
        std::cout << "main() -- req.body.size :: " << req.body.size() << std::endl;
        std::cout << "main() -- contentLength :: " << contentLength << std::endl;
        if (req.body.size() < contentLength) {
            std::string rest = readAll(cfd, contentLength - req.body.size());
            req.body += rest;
        }

        std::cerr << req.method << " " << req.path << " (" << req.body.size() << " bytes)\n";

        std::string transferEnc = toLower(getHeader(req, "transfer-encoding"));

        std::cout << "main() transferEnc=" << transferEnc << std::endl;

        if (!transferEnc.empty() && transferEnc.find("chunked") != std::string::npos) {
            std::string resp = tianshan_http.makeResponse(501, "Not Implemented", "application/json", "{\n  \"ok\": false, \"error\": \"chunked transfer not supported\"\n}\n");
            sendAll(cfd, resp);
            ::close(cfd);
            continue;
        }

        std::cout << "main() http method=" << req.method << ",path=" << req.path << std::endl;
        std::string response;
        if (req.method == "GET" && req.path == "/") {
            std::string html =
                "<!doctype html>\n<html><head><meta charset=\"utf-8\"><title>Upload</title></head>"
                "<body><h1>Upload</h1>"
                "<form method=\"POST\" action=\"/upload\" enctype=\"multipart/form-data\">"
                "<input type=\"file\" name=\"file\" />"
                "<button type=\"submit\">Upload</button>"
                "</form></body></html>";
            response = tianshan_http.makeResponse(200, "OK", "text/html; charset=utf-8", html);
        } else if (req.method == "POST" && req.path == "/upload") {
            fs::path saved;
            size_t bytes = 0;
//            std::string ctype = toLower(getHeader(req, "content-type"));
            std::string ctype = getHeader(req, "content-type");
            std::cout << "main() ctype=" << ctype << std::endl;
            bool ok = false;
            if (ctype.find("multipart/form-data") != std::string::npos) {
                ok = handleMultipart(ctype, req.body, saved, bytes);
                if (!ok) {
                    std::string body = "{\n  \"ok\": false, \"error\": \"invalid multipart form data\"\n}\n";
                    response = tianshan_http.makeResponse(400, "Bad Request", "application/json", body);
                }
            }
            if (!ok) {
                // treat entire body as file
                std::string filename = generateFileName("bin");
                if (saveToFile(yaml.uploadFolder, CUploadService::get_filename(), req.body, saved)) {
                    bytes = req.body.size();
                    ok = true;
                } else {
                    std::string body = "{\n  \"ok\": false, \"error\": \"failed to save file\"\n}\n";
                    response = tianshan_http.makeResponse(500, "Internal Server Error", "application/json", body);
                }
            }
            if (ok) {
                std::ostringstream json;
                json << "{\n  \"ok\": true, \"filename\": \"" << saved.string() << "\", \"bytes\": " << bytes << "\n}\n";
                response = tianshan_http.makeResponse(200, "OK", "application/json", json.str());
                std::cerr << "Saved file: " << saved << " (" << bytes << " bytes)\n";
            }
        } else if (req.method == "GET" || req.method == "POST" || req.method == "HEAD" || req.method == "PUT" || req.method == "DELETE") {
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
