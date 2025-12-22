//
// Created by developer on 2025-12-21.
//

#ifndef FILEUPLOAD_CTIANSHANHTTPRESPONSE_H
#define FILEUPLOAD_CTIANSHANHTTPRESPONSE_H
#include <string>


class CTianshanHttpResponse {
private:
    std::string body;

public:
    CTianshanHttpResponse(std::string response):body(response){};
    std::string getBody(){return this->body;}
};

#endif //FILEUPLOAD_CTIANSHANHTTPRESPONSE_H

