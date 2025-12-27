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

class CTianshanConfig {
private:
    std::string host;
    int port;
    std::string uploadFolder;
    std::string templateFolder;
    std::string publicFolder;
    std::string publicPrefix;
public:
    void load_config();

    [[nodiscard]] std::string getUploadFolder() const {
        return uploadFolder;
    }

    [[nodiscard]] std::string getTemplateFolder() const {
        return templateFolder;
    }

    [[nodiscard]] std::string getHost() const {
        return host;
    }

    [[nodiscard]] int getPort() const {
        return port;
    }

    [[nodiscard]] std::string public_folder() const {
        return publicFolder;
    }

    [[nodiscard]] std::string public_prefix() const {
        return publicPrefix;
    }
};


#endif //FILEUPLOAD_CUPLOADSERVICE_H