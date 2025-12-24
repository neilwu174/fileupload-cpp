//
// Created by developer on 2025-12-21.
//

#ifndef FILEUPLOAD_CTIANSHANHTTPRESPONSE_H
#define FILEUPLOAD_CTIANSHANHTTPRESPONSE_H
#include <map>
#include <string>


class CTianshanHttpResponse {
private:
    int status = 200;
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
};

#endif //FILEUPLOAD_CTIANSHANHTTPRESPONSE_H

