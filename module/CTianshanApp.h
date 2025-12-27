//
// Created by developer on 2025-12-26.
//

#ifndef FILEUPLOAD_CTIANSHANAPP_H
#define FILEUPLOAD_CTIANSHANAPP_H
#include <csignal>
#include <iostream>
#include <unistd.h>

#include "CTianshanConfig.h"
#include "CTianshanApp.h"

class CTianshanApp {
private:
    CTianshanConfig &config;
    // static volatile std::sig_atomic_t g_shouldStop;
    // static int g_listenFd;

public:
    CTianshanApp(CTianshanConfig &config):config(config) {};
    int run();
};


#endif //FILEUPLOAD_CTIANSHANAPP_H