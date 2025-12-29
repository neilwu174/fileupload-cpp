cmake_minimum_required(VERSION 4.0)
project(fileupload)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

FetchContent_Declare(
        nlohmann
        GIT_REPOSITORY git@github.com:nlohmann/json.git
        GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)

FetchContent_MakeAvailable(nlohmann)

# Find the CURL package
find_package(CURL REQUIRED)

add_executable(fileupload main.cpp
        module/CTianshanConfig.cpp
        module/CTianshanConfig.h
        module/CTianshanHttpResponseHandler.cpp
        module/CTianshanHttpResponseHandler.h
        module/CTianshanHttpRequest.cpp
        module/CTianshanHttpRequest.h
        module/CTianshanMultipartHandler.cpp
        module/CTianshanMultipartHandler.h
        module/CTianshanHttpController.cpp
        module/CTianshanHttpController.h
        module/tianshan_functions.h
        module/CTianshanHttpResponse.cpp
        module/CTianshanHttpResponse.h
        module/inja.h
        module/CTianshanHtmlHandler.cpp
        module/CTianshanHtmlHandler.h
        module/CTianshanAbstractHandler.h
        module/file_utils.h
        module/CTianshanHtmlHomeHandler.cpp
        module/CTianshanHtmlHomeHandler.h
        module/CTianshanApp.cpp
        module/CTianshanApp.h
        module/CTianshanFilesystem.cpp
        module/CTianshanFilesystem.h
        module/CTianshanHtmlNotFoundHandler.cpp
        module/CTianshanHtmlNotFoundHandler.h
        module/CTianshanFile.cpp
        module/CTianshanFile.h
        module/models.h
        module/CTianshanHtmlFolderHandler.cpp
        module/CTianshanHtmlFolderHandler.h
)

target_link_libraries(fileupload PUBLIC yaml-cpp::yaml-cpp nlohmann_json::nlohmann_json) # The library or executable that require yaml-cpp library
# Link your executable against the imported libcurl target
target_link_libraries(fileupload PRIVATE CURL::libcurl)
