//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_CTIANSHANWEBHANDLER_H
#define FILEUPLOAD_CTIANSHANWEBHANDLER_H
#include "CTianshanAbstractHandler.h"


class CTianshanHtmlHandler: public CTianshanAbstractHandler {
public:
    CTianshanHtmlHandler(CTianshanConfig &config) : CTianshanAbstractHandler(config) {};
    CTianshanHttpResponse accept(CTianshanHttpRequest& request) override;
};


#endif //FILEUPLOAD_CTIANSHANWEBHANDLER_H