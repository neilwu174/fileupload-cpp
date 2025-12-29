//
// Created by developer on 2025-12-18.
//

#ifndef FILEUPLOAD_CTIANSHANHTTPREQUEST_H
#define FILEUPLOAD_CTIANSHANHTTPREQUEST_H


#include <map>
#include <string>

#include "CTianshanHttpResponseHandler.h"

static inline std::string toLower(std::string s) {
    for (auto &c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

class CTianshanHttpRequest {
private:
    size_t maxBytes = 64 * 1024;
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string,std::string> headers; // lower-cased keys
    std::map<std::string,std::string> queryParameters; // lower-cased keys
    std::string body;
    size_t headerEnd;
private:
    void parseQueryParameters(const std::string &queryString);
    bool parseRequest(const std::string &raw);
    bool readHeaders(int fd, std::string &raw);

public:
    inline static const std::string HTTP_POST = "POST";
    inline static const std::string HTTP_GET = "GET";

public:
    CTianshanHttpRequest() = default;

    std::string getMethod() { return method; }

    bool route(const char * str, const char * text);;
    std::string getPath() { return path; };
    std::string getVersion() { return version; };
    std::map<std::string,std::string> getHeaders() { return headers; };
    std::map<std::string,std::string> getQueryParameters() { return queryParameters; };
    std::string getQueryParameter(const std::string &key);
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

    bool accept(int fd);
    std::string getHeader(const std::string &key);
};


#endif //FILEUPLOAD_CTIANSHANHTTPREQUEST_H