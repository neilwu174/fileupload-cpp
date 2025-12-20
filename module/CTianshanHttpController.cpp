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

    CTianshanHttpRequest httpRequest;
    CTianshanHttpResponseHandler httpResponse;

    if (!httpRequest.accept(incoming)) {
        std::string resp = httpResponse.makeResponse(400, "Bad httpRequestuest", "text/plain; charset=utf-8", "Malformed headers\n");
        sendAll(incoming, resp);
        ::close(incoming);
        return;
    }

    // std::string transferEnc = toLower(httpRequest.getHeader("transfer-encoding"));
    // if (!transferEnc.empty() && transferEnc.find("chunked") != std::string::npos) {
    //     std::string resp = httpResponse.makeResponse(501, "Not Implemented", "application/json", "{\n  \"ok\": false, \"error\": \"chunked transfer not supported\"\n}\n");
    //     sendAll(incoming, resp);
    //     ::close(incoming);
    //     return;
    // }

    std::string response;
    if (httpRequest.route("GET","/")) {
        std::string html =
            "<!doctype html>\n<html><head><meta charset=\"utf-8\"><title>Upload</title></head>"
            "<body><h1>Upload</h1>"
            "<form method=\"POST\" action=\"/upload\" enctype=\"multipart/form-data\">"
            "<input type=\"file\" name=\"file\" />"
            "<button type=\"submit\">Upload</button>"
            "</form></body></html>";
        response = httpResponse.makeResponse(200, "OK", "text/html; charset=utf-8", html);
    } else if (httpRequest.route("POST","/upload")) {
        CTianshanMultipartHandler handler(config.getUploadFolder());
        response = handler.accept(httpRequest);
    } else {
        response = httpResponse.makeResponse(404, "Not Found", "text/plain; charset=utf-8", "Not Found\n");
    }

    sendAll(incoming, response);
    ::close(incoming);
}
