//
// Created by developer on 2025-12-27.
//

#include "CTianshanFilesystem.h"

#include <filesystem>
#include <iostream>
#include <curl/curl.h>

void CTianshanFilesystem::scan(std::string &path) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::cout << entry.path() << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool CTianshanFilesystem::downloadFile(const std::string &url, const std::string &localPath) {
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(localPath.c_str(), "wb");
        if (!fp) {
            std::cerr << "Failed to open file for writing: " << localPath << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects

        res = curl_easy_perform(curl);
        
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res == CURLE_OK) {
            return true;
        } else {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
    }
    return false;
}
