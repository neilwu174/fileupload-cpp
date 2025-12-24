//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_CTIANSHANWEBHANDLER_H
#define FILEUPLOAD_CTIANSHANWEBHANDLER_H
#include "CTianshanAbstractHandler.h"


class CTianshanWebHandler: public CTianshanAbstractHandler {
public:
    CTianshanHttpResponse accept(CTianshanHttpRequest& request) override;
};


#endif //FILEUPLOAD_CTIANSHANWEBHANDLER_H