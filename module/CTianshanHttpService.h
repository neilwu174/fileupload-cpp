//
// Created by developer on 2025-12-17.
//

#ifndef FILEUPLOAD_CUPLOADSERVICE_H
#define FILEUPLOAD_CUPLOADSERVICE_H
#include <map>
#include <string>

struct HttpConfig {
    std::string uploadFolder;
    std::string host;
    int port;
};

class CTianshanHttpService {
public:
    void load_config(HttpConfig& config);
};


#endif //FILEUPLOAD_CUPLOADSERVICE_H