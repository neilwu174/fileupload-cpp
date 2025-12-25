//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_CTIANSHANWEBHANDLER_H
#define FILEUPLOAD_CTIANSHANWEBHANDLER_H
#include "CTianshanAbstractHandler.h"
#include "inja.h"


class CTianshanHtmlHandler: public CTianshanAbstractHandler {
protected:
    std::string get_html_file(inja::Environment& env,inja::json& model,std::string templateFolder,std::string file_name);
public:
    CTianshanHtmlHandler(CTianshanConfig &config) : CTianshanAbstractHandler(config) {};
    CTianshanHttpResponse accept(CTianshanHttpRequest& request) override;
};


#endif //FILEUPLOAD_CTIANSHANWEBHANDLER_H