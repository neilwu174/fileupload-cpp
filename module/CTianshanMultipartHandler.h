//
// Created by developer on 2025-12-18.
//

#ifndef FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H
#define FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H
#include <filesystem>
#include <string>

#include "CTianshanAbstractHandler.h"
#include "CTianshanHttpRequest.h"
#include "CTianshanHttpResponse.h"


class CTianshanMultipartHandler : public CTianshanAbstractHandler {

private:
    size_t bytesSaved;
private:
    std::string get_filename();
    long get_miiliseconds();
    std::string generateFileName(const std::string &ext = "bin");
    bool acceptInternal(CTianshanHttpRequest& request, std::filesystem::path &savedPath, size_t &bytesSaved);

public:
    CTianshanMultipartHandler(CTianshanConfig &config) : CTianshanAbstractHandler(config) {};
    std::string extractParam(const std::string &headerValue, const std::string &param);
    bool saveToFile(const std::string &filename, const std::string &data, std::filesystem::path &outPath);
    CTianshanHttpResponse accept(CTianshanHttpRequest& request) override;
};


#endif //FILEUPLOAD_CTIANSHANMULTIPARTHANDLER_H