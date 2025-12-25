//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_CTIANSHANHTMLHOMEHANDLER_H
#define FILEUPLOAD_CTIANSHANHTMLHOMEHANDLER_H
#include "CTianshanHtmlHandler.h"


class CTianshanHtmlHomeHandler: public CTianshanHtmlHandler {
protected:
    std::string get_file_name() override {
        return "index";
    };
    inja::json get_model() override {
        inja::json data;
        data["title"] = "Welcome to My Website";
        data["user_name"] = "Alice";
        data["items"] = {"Item A", "Item B", "Item C"};
        return data;
    };

public:
    CTianshanHtmlHomeHandler(CTianshanConfig &config) : CTianshanHtmlHandler(config) {};
};


#endif //FILEUPLOAD_CTIANSHANHTMLHOMEHANDLER_H