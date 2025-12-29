//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_CTIANSHANWEBHANDLER_H
#define FILEUPLOAD_CTIANSHANWEBHANDLER_H
#include "CTianshanAbstractHandler.h"
#include "inja.h"


class CTianshanHtmlHandler: public CTianshanAbstractHandler {
protected:
    virtual
    inja::json get_model(std::map<std::string,std::string>& parameters) = 0;
    virtual
    std::string get_file_name() = 0;
public:
    CTianshanHtmlHandler(CTianshanConfig &config) : CTianshanAbstractHandler(config) {};
    CTianshanHttpResponse accept(CTianshanHttpRequest& request) override;
};


#endif //FILEUPLOAD_CTIANSHANWEBHANDLER_H