//
// Created by developer on 2025-12-19.
//

#include "CTianshanHttpController.h"

#include <iostream>
#include <string>
#include <unistd.h>

#include "CTianshanHttpRequest.h"
#include "CTianshanMultipartHandler.h"

void CTianshanHttpController::accept(int incoming,CTianshanConfig& config) {
        std::string raw;
        CTianshanHttpRequest req;
        CTianshanHttpResponseHandler tianshan_http;
        if (!req.readHeaders(incoming, raw)) {
            std::string resp = tianshan_http.makeResponse(400, "Bad Request", "text/plain; charset=utf-8", "Malformed headers\n");
            sendAll(incoming, resp);
            ::close(incoming);
            return;
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
            std::string rest = readAll(incoming, contentLength - req.getBody().size());
            req.setBody(req.getBody() + rest);
        }

        std::string transferEnc = toLower(req.getHeader("transfer-encoding"));
        if (!transferEnc.empty() && transferEnc.find("chunked") != std::string::npos) {
            std::string resp = tianshan_http.makeResponse(501, "Not Implemented", "application/json", "{\n  \"ok\": false, \"error\": \"chunked transfer not supported\"\n}\n");
            sendAll(incoming, resp);
            ::close(incoming);
            return;
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
                CTianshanMultipartHandler handler(config.getUploadFolder());
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

        sendAll(incoming, response);
        ::close(incoming);

}
