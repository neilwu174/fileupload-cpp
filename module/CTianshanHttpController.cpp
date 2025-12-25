//
// Created by developer on 2025-12-19.
//

#include "CTianshanHttpController.h"

#include <iostream>
#include <string>
#include <unistd.h>

#include "CTianshanHttpRequest.h"
#include "CTianshanMultipartHandler.h"

void CTianshanHttpController::proceed(int incoming) {

    CTianshanHttpRequest httpRequest;
    CTianshanHttpResponseHandler httpResponse;

    if (!httpRequest.accept(incoming)) {
        std::string resp = httpResponse.makeResponse(400, "Bad httpRequestuest", "text/plain; charset=utf-8", "Malformed headers\n");
        sendAll(incoming, resp);
        ::close(incoming);
        return;
    }

    std::string routeKey = httpRequest.getMethod() + "-" + httpRequest.getPath();
    std::string response;

    if (this->routes.count(routeKey)) {
        auto handler = this->routes[routeKey];
        CTianshanHttpResponse http_response = handler(httpRequest);
        response = http_response.build();
    } else {
        response = httpResponse.makeResponse(404, "Not Found", "text/plain; charset=utf-8", "Not Found\n");
    }
    sendAll(incoming, response);
    ::close(incoming);
}

void CTianshanHttpController::route(const char *method, const char *path, std::function<CTianshanHttpResponse(CTianshanHttpRequest &)> httpHandler) {
    this->routes.emplace(std::string(method) + "-" + std::string(path), httpHandler);
}
