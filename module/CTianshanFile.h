//
// Created by developer on 2025-12-28.
//

#ifndef FILEUPLOAD_CTIANSHANFILE_H
#define FILEUPLOAD_CTIANSHANFILE_H
#include <string>


class CTianshanFile {
private:
    std::string image;
    std::string parent;
    std::string path;
    std::string name;
    std::string kind;
    long size;
    std::string modified_date;
};


#endif //FILEUPLOAD_CTIANSHANFILE_H