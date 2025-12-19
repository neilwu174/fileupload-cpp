//
// Created by developer on 2025-12-18.
//

#ifndef FILEUPLOAD_CTIANSHANHTTPREQUEST_H
#define FILEUPLOAD_CTIANSHANHTTPREQUEST_H


#include <map>
#include <string>

#include "CTianshanHttpResponseHandler.h"

class CTianshanHttpRequest {
private:
    size_t maxBytes = 64 * 1024;
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string,std::string> headers; // lower-cased keys
    std::string body;
    size_t headerEnd;
private:
    bool parseRequest(const std::string &raw);

public:
    CTianshanHttpRequest() = default;

    std::string getMethod() { return method; };
    std::string getPath() { return path; };
    std::string getVersion() { return version; };
    std::map<std::string,std::string> getHeaders() { return headers; };
    std::string getBody() { return body; };
    size_t getHeaderEnd() { return headerEnd; };
    void setBody(std::string body) { this->body = body; };
    void setHeader(std::string key, std::string value) { headers[toLower(key)] = value; };
    void setMethod(std::string method) { this->method = method; };
    void setPath(std::string path) { this->path = path; };
    void setVersion(std::string version) { this->version = version; };

    std::string getContentType() {
        return getHeader("content-type");
    }

    bool readHeaders(int fd, std::string &raw);
    std::string getHeader(const std::string &key);
};


#endif //FILEUPLOAD_CTIANSHANHTTPREQUEST_H