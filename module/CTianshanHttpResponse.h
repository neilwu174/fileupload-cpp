//
// Created by developer on 2025-12-21.
//

#ifndef FILEUPLOAD_CTIANSHANHTTPRESPONSE_H
#define FILEUPLOAD_CTIANSHANHTTPRESPONSE_H
#include <string>


class CTianshanHttpResponse {
private:
    std::string response;

public:
    CTianshanHttpResponse(std::string response):response(response){};
    std::string getResponse(){return this->response;}
};

#endif //FILEUPLOAD_CTIANSHANHTTPRESPONSE_H