//
// Created by developer on 2025-12-21.
//

#ifndef FILEUPLOAD_CTIANSHANHTTPRESPONSE_H
#define FILEUPLOAD_CTIANSHANHTTPRESPONSE_H
#include <map>
#include <string>

const int HTTP_STATUS_OK = 200;
const int HTTP_STATUS_NOT_FOUND = 404;

class CTianshanHttpResponse {
private:
    int status = HTTP_STATUS_OK;
    std::string statusText = "OK";
    std::string date;
    std::string agent;
    std::string contentType = "text/html";
    std::map<std::string,std::string> headers;
    std::string body;

public:
    CTianshanHttpResponse(
        int status,
        std::string statusText,
        std::string agent,
        std::string contentType,
        std::string body):
    status(status),
    statusText(statusText),
    agent(agent),
    contentType(contentType),
    body(body){};

    std::string build();
    void setHeader(std::string key,std::string value) {
        this->headers[key] = value;
    }
    void setStatus(int status) {
        this->status = status;
    }
};

#endif //FILEUPLOAD_CTIANSHANHTTPRESPONSE_H

