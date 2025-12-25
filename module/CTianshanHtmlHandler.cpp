//
// Created by developer on 2025-12-24.
//

#include "CTianshanHtmlHandler.h"

#include <iostream>

#include "file_utils.h"
#include "inja.h"

std::string CTianshanHtmlHandler::get_html_file(inja::Environment& env,inja::json& model,std::string templateFolder, std::string file_name) {
    std::string full_name = file_name + ".html";
    fs::path template_path = this->getConfig().getTemplateFolder();
    fs::path file_path = template_path  / "html" / full_name;
    std::string html_template = readFileContent(file_path);
    std::string result = env.render(html_template, model);
    return result;
}

CTianshanHttpResponse CTianshanHtmlHandler::accept(CTianshanHttpRequest &request) {

    inja::Environment env;

    // 1. Define your data using a JSON object
    inja::json data;
    data["title"] = "Welcome to My Website";
    data["user_name"] = "Alice";
    data["items"] = {"Item A", "Item B", "Item C"};
    std::string result = get_html_file(env,data,this->getConfig().getTemplateFolder(),"index");
    // 3. Render the template with the data
    CTianshanHttpResponse http_response(200,"OK","TianshanWebAgent","text/html",result);
    // 4. Output the final HTML
    std::cout << result << std::endl;
    return http_response;
}
