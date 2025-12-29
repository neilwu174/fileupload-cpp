//
// Created by developer on 2025-12-28.
//

#ifndef FILEUPLOAD_MODELS_H
#define FILEUPLOAD_MODELS_H
#include <string>
struct TianshanFile {
    std::string image;
    std::string parent;
    std::string path;
    std::string name;
    std::string kind;
    long size;
    std::string modified_date;
};
#endif //FILEUPLOAD_MODELS_H