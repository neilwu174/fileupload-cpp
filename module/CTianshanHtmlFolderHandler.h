//
// Created by developer on 2025-12-29.
//

#ifndef FILEUPLOAD_CTIANSHANHTMLFOLDERHANDLER_H
#define FILEUPLOAD_CTIANSHANHTMLFOLDERHANDLER_H
#include <string>
#include <algorithm>
#include <filesystem>
#include "CTianshanHtmlHandler.h"
#include "file_utils.h"
#include "inja.h"
#include "models.h"


class CTianshanHtmlFolderHandler: public CTianshanHtmlHandler {

protected:
    std::string get_file_name() override {
        return "folder";
    };
    inja::json get_model(std::map<std::string,std::string>& parameters) override {
        std::string folder_path = parameters["name"];
        inja::json data;
        std::filesystem::path p(folder_path);
        data["folders"] = inja::json::array();
        data["parent"] = p.parent_path().string();
        data["current"] = folder_path;
        // std::vector<TianshanFile> file_vector = {
        //     {"folder.png", folder_path, "/Users/developer/documents", "documents", "directory", 0, ""},
        //     {"folder.png", folder_path, "/Users/developer/downloads", "download", "directory", 0, ""},
        //     {"file.png", folder_path, "path002", "Bob", "file", 25, ""},
        //     {"file.png", folder_path, "path003", "Charlie", "file", 35, ""}
        // };
        std::vector<TianshanFile> file_vector = scan_directory(folder_path);

        std::sort(file_vector.begin(), file_vector.end(), [](const TianshanFile& a, const TianshanFile& b) {
            if (a.kind != b.kind) {
                return a.kind == "directory";
            }
            return a.name < b.name;
        });

        data["files"] = inja::json::array();
        for (const auto& file : file_vector) {
            std::cout << "get_model() " << file.name << std::endl;
            try {
                if (file.kind == "directory") {
                    data["folders"].push_back({
                        {"path", file.path},
                        {"name", file.name}
                    });
                } else {
                    data["files"].push_back({
                        {"image", file.image},
                        {"size", file.size},
                        {"name", file.name},
                        {"path", file.path},
                        {"kind", file.kind},
                        {"modified_date", file.modified_date}
                    });
                }
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Filesystem error caught:" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "General exception caught: " << e.what() << std::endl;
            }
        }
        return data;
    };

public:
    // CTianshanHtmlFolderHandler(CTianshanConfig &config,std::string folder) : CTianshanHtmlHandler(config), folder_path(folder) {};
    CTianshanHtmlFolderHandler(CTianshanConfig &config) : CTianshanHtmlHandler(config) {};
};


#endif //FILEUPLOAD_CTIANSHANHTMLFOLDERHANDLER_H