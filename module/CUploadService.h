//
// Created by developer on 2025-12-17.
//

#ifndef FILEUPLOAD_CUPLOADSERVICE_H
#define FILEUPLOAD_CUPLOADSERVICE_H
#include <string>

struct HttpConfig {
    std::string uploadFolder;
    std::string host;
    int port;
};

class CUploadService {
public:
    void load_config(HttpConfig& config);
    static std::string get_filename();
private:
    static long get_miiliseconds();
};


#endif //FILEUPLOAD_CUPLOADSERVICE_H