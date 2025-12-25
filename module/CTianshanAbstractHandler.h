//
// Created by developer on 2025-12-24.
//

#ifndef FILEUPLOAD_CTIANSHANABSTRACTHANDLER_H
#define FILEUPLOAD_CTIANSHANABSTRACTHANDLER_H
#include "CTianshanConfig.h"
#include "CTianshanHttpRequest.h"
#include "CTianshanHttpResponse.h"

class CTianshanAbstractHandler {
private:
    CTianshanConfig& config;
public:
    CTianshanAbstractHandler(CTianshanConfig& config):config(config){};

    virtual
    CTianshanHttpResponse accept(CTianshanHttpRequest& request) = 0;

    [[nodiscard]] CTianshanConfig& getConfig() const {
        return config;
    }
    void setConfig(const CTianshanConfig &config) {
        this->config = config;
    }
};
#endif //FILEUPLOAD_CTIANSHANABSTRACTHANDLER_H