//
// Created by developer on 2025-12-19.
//

#include "CTianshanHttpController.h"

#include <iostream>
#include <string>
#include <unistd.h>

#include "CTianshanHttpRequest.h"
#include "CTianshanMultipartHandler.h"
#include "file_utils.h"

void CTianshanHttpController::proceed(int incoming) {

    CTianshanHttpRequest httpRequest;
    CTianshanHttpResponseHandler httpResponse;

    if (!httpRequest.accept(incoming)) {
        std::string resp = httpResponse.makeResponse(400, "Bad httpRequestuest", "text/plain; charset=utf-8", "Malformed headers\n");
        sendAll(incoming, resp);
        ::close(incoming);
        return;
    }

    //Handle static css/js/images
    if (httpRequest.getPath().rfind(this->config.public_prefix(),0) == 0) {
        //a static page
        std::string filename = httpRequest.getPath().substr(this->config.public_prefix().size());
        std::cout << "filename=" << filename << std::endl;

        fs::path path = this->config.public_folder() + filename;
        std::cout << "path=" << path << std::endl;
        std::string content = readFileContent(path);
        std::string resp = httpResponse.makeResponse(200, "Ok", createContentType(filename), content);
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
        routeKey = "GET-/not-found";
        auto handler = this->routes[routeKey];
        CTianshanHttpResponse http_response = handler(httpRequest);
        response = http_response.build();
        // response = httpResponse.makeResponse(404, "Not Found", "text/plain; charset=utf-8", "Not Found\n");
    }
    sendAll(incoming, response);
    ::close(incoming);
}

void CTianshanHttpController::route(const char *method, const char *path, std::function<CTianshanHttpResponse(CTianshanHttpRequest &)> httpHandler) {
    this->routes.emplace(std::string(method) + "-" + std::string(path), httpHandler);
}
