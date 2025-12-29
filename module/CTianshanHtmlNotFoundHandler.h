//
// Created by developer on 2025-12-28.
//

#ifndef FILEUPLOAD_CTIANSHANHTMLNOTFOUNDHANDLER_H
#define FILEUPLOAD_CTIANSHANHTMLNOTFOUNDHANDLER_H
#include <string>

#include "CTianshanConfig.h"
#include "CTianshanHtmlHandler.h"
#include "inja.h"


class CTianshanHtmlNotFoundHandler: public CTianshanHtmlHandler {
protected:
    std::string get_file_name() override {
        return "not_found";
    };
    inja::json get_model() override {
        inja::json data;
        data["title"] = "Welcome to My Website";
        data["user_name"] = "Alice";
        data["items"] = {"Item A", "Item B", "Item C"};
        return data;
    };

public:
    CTianshanHtmlNotFoundHandler(CTianshanConfig &config) : CTianshanHtmlHandler(config) {};
    CTianshanHttpResponse accept(CTianshanHttpRequest& request) override {
        CTianshanHttpResponse response = CTianshanHtmlHandler::accept(request);
        response.setStatus(HTTP_STATUS_NOT_FOUND);
        return response;
    };
};


#endif //FILEUPLOAD_CTIANSHANHTMLNOTFOUNDHANDLER_H