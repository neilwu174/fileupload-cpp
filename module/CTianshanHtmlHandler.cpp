//
// Created by developer on 2025-12-24.
//

#include "CTianshanHtmlHandler.h"

#include <iostream>

#include "inja.h"

CTianshanHttpResponse CTianshanHtmlHandler::accept(CTianshanHttpRequest &request) {

    inja::Environment env;

    // 1. Define your data using a JSON object
    inja::json data;
    data["title"] = "Welcome to My Website";
    data["user_name"] = "Alice";
    data["items"] = {"Item A", "Item B", "Item C"};

    // 2. Define your HTML template (can also be loaded from a file)
    std::string html_template = R"(
    <!DOCTYPE html>
    <html>
    <head>
        <title>{{ title }}</title>
    </head>
    <body>
        <h1>Hello, {{ user_name }}!</h1>
        <ul>
        {% for item in items %}
            <li>{{ item }}</li>
        {% endfor %}
        </ul>
    </body>
    </html>
    )";

    // 3. Render the template with the data
    std::string result = env.render(html_template, data);
    CTianshanHttpResponse http_response(200,"OK","TianshanWebAgent","text/html",result);
    // 4. Output the final HTML
    std::cout << result << std::endl;
    return http_response;
}
