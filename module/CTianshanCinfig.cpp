//
// Created by developer on 2025-12-17.
//

#include "CTianshanCinfig.h"

#include <iostream>

#include "yaml-cpp/yaml.h"

/*
 * public method
 */
void CTianshanCinfig::load_config(HttpConfig& yaml) {
    YAML::Node config = YAML::LoadFile("../application.yaml");
    std::string uploadFolder = config["upload"].as<std::string>();
    std::cout << "uploadFolder: " << uploadFolder << std::endl;

    // Access nested values (map within a map)
    std::string db_host = config["server"]["host"].as<std::string>();
    int db_port = config["server"]["port"].as<int>();
    std::cout << "Server Host: " << db_host << ", Port: " << db_port << std::endl;
    // HttpConfig yaml = {uploadFolder, db_host, std::to_string(db_port)};
    yaml.port = db_port;
    yaml.uploadFolder = uploadFolder;
    yaml.host = db_host;
}
