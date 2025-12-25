//
// Created by developer on 2025-12-24.
//

#include "CTianshanHtmlHandler.h"

#include <iostream>

#include "file_utils.h"
#include "inja.h"

CTianshanHttpResponse CTianshanHtmlHandler::accept(CTianshanHttpRequest &request) {

    inja::Environment env;
    std::string full_name = this->get_file_name() + ".html";
    inja::json model = this->get_model();
    fs::path template_path = this->getConfig().getTemplateFolder();
    fs::path file_path = template_path  / "html" / full_name;
    std::string html_template = readFileContent(file_path);
    std::string result = env.render(html_template, model);
    CTianshanHttpResponse http_response(200,"OK","TianshanWebAgent","text/html",result);
    return http_response;
}
