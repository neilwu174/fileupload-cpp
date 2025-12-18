//
// Created by developer on 2025-12-17.
//

#include "CUploadService.h"

#include <iostream>

#include "yaml-cpp/yaml.h"

/*
 * public method
 */
void CUploadService::load_config(HttpConfig& yaml) {
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

std::string CUploadService::get_filename() {
    long current_time = get_miiliseconds();
    std::string filename = "photo-" + std::to_string(current_time) + ".jpeg";
    return filename;
}

/*
 * private method
 */
long CUploadService::get_miiliseconds() {
    // Get the current time point from the system clock
    auto now = std::chrono::system_clock::now();

    // Cast the duration since the epoch to milliseconds and get the count
    auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();

    std::cout << "Milliseconds since epoch: " << milliseconds_since_epoch << std::endl;
    return milliseconds_since_epoch;
}
