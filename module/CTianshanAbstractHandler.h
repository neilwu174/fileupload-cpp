//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_CTIANSHANABSTRACTHANDLER_H
#define FILEUPLOAD_CTIANSHANABSTRACTHANDLER_H
#include "CTianshanHttpRequest.h"
#include "CTianshanHttpResponse.h"

class CTianshanAbstractHandler {
public:
    virtual
    CTianshanHttpResponse accept(CTianshanHttpRequest& request) = 0;
};
#endif //FILEUPLOAD_CTIANSHANABSTRACTHANDLER_H