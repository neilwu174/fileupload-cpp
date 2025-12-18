//
// Created by developer on 2025-12-18.
//

#include "CTianshanMultipartHandler.h"

#include <filesystem>
#include <iostream>
#include <map>
#include <ostream>
#include <string>

#include "CTianshanHttp.h"
#include "CUploadService.h"

bool CTianshanMultipartHandler::saveToFile(const std::string &filename, const std::string &data, fs::path &outPath) {
    try {
        fs::create_directories(uploadDir);
        outPath = uploadDir / filename;
        std::ofstream ofs(outPath, std::ios::binary);
        if (!ofs) return false;
        ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
        return ofs.good();
    } catch (...) {
        return false;
    }
}

std::string CTianshanMultipartHandler::extractParam(const std::string &headerValue, const std::string &param) {
    // naive param extractor: param="value"
    std::string key = param + "=";
    auto pos = headerValue.find(key);
    if (pos == std::string::npos) return {};
    pos += key.size();
    if (pos < headerValue.size() && headerValue[pos] == '"') {
        auto end = headerValue.find('"', pos + 1);
        if (end != std::string::npos) return headerValue.substr(pos + 1, end - (pos + 1));
    } else {
        // token form until ; or end
        auto end = headerValue.find(';', pos);
        return headerValue.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
    }
    return {};
}

bool CTianshanMultipartHandler::handle(
    const std::string &contentType,
    const std::string &body,
    std::filesystem::path &savedPath,
    size_t &bytesSaved) {

    auto bpos = contentType.find("boundary=");
    if (bpos == std::string::npos) return false;
    std::string boundary = contentType.substr(bpos + 9);
    if (!boundary.empty() && boundary.front() == '"' && boundary.back() == '"') {
        boundary = boundary.substr(1, boundary.size() - 2);
    }
    std::string sep = "--" + boundary;
    std::string endSep = sep + "--";

    std::cout << "handleMultipart(started)-sep=" << sep << std::endl;
    std::cout << "handleMultipart(started)-endSep=" << endSep << std::endl;

    // Split parts by boundary lines
    size_t pos = 0;
    bool foundFile = false;
    std::cout << "handleMultipart(loop)" << std::endl;
    while (true) {
        size_t start = body.find(sep, pos);
        std::cout << "handleMultiPart() start=" << start << std::endl;
        std::cout << "handleMultiPart() std::string::npos=" << std::string::npos << std::endl;

        if (start == std::string::npos) break;
        start += sep.size();
        if (start < body.size() && body[start] == '\r') start++;
        if (start < body.size() && body[start] == '\n') start++;
        size_t next = body.find(sep, start);
        if (next == std::string::npos) next = body.find(endSep, start);
        if (next == std::string::npos) break;
        size_t partEnd = next;
        // Trim trailing CRLF
        if (partEnd >= 1 && body[partEnd-1] == '\n') partEnd--;
        if (partEnd >= 1 && body[partEnd-1] == '\r') partEnd--;

        // part headers/body split
        size_t hEnd = body.find("\r\n\r\n", start);
        if (hEnd == std::string::npos || hEnd > partEnd) { pos = next; continue; }
        std::string pHeaders = body.substr(start, hEnd - start);
        std::string pBody = body.substr(hEnd + 4, partEnd - (hEnd + 4));

        std::cout << "handleMultiPart() pHeaders=" << pHeaders << std::endl;
//        std::cout << "handleMultiPart() pBody=" << pBody << std::endl;

        // parse headers
        std::istringstream ph(pHeaders);
        std::string hline;
        std::map<std::string,std::string> hmap;
        while (std::getline(ph, hline)) {

            std::cout << "handleMultiPart() hline=" << hline << std::endl;

            if (!hline.empty() && hline.back() == '\r') hline.pop_back();
            auto c = hline.find(':');
            if (c == std::string::npos) continue;
            std::string k = toLower(hline.substr(0, c));
            std::string v = hline.substr(c + 1);
            size_t s = v.find_first_not_of(" \t");
            if (s != std::string::npos) v.erase(0, s);
            while (!v.empty() && (v.back() == ' ' || v.back() == '\t')) v.pop_back();
            hmap[k] = v;
        }
        auto cdIt = hmap.find("content-disposition");
        if (cdIt != hmap.end()) {
            std::string cd = cdIt->second;
            std::string filename = extractParam(cd, "filename");
            if (!filename.empty()) {
                // sanitize filename: keep only basename
                try {
                    filename = fs::path(filename).filename().string();
                } catch (...) {
                    // fallback
                }
                if (filename.empty()) filename = generateFileName("bin");
                fs::path out;
                if (this->saveToFile(CUploadService::get_filename(), pBody, out)) {
                    savedPath = out;
                    bytesSaved = pBody.size();
                    foundFile = true;
                    break;
                }
            }
        }
        pos = next;
    }
    return foundFile;
}
