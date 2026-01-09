cmake_minimum_required(VERSION 4.0)
project(fileupload)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET "26.0" CACHE STRING "Minimum OS X deployment version")

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

FetchContent_Declare(
        nadjieb_mjpeg_streamer
        GIT_REPOSITORY git@github.com:nadjieb/cpp-mjpeg-streamer.git
        GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(nadjieb_mjpeg_streamer)

FetchContent_Declare(
        websocket
        GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
        GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(websocket)

FetchContent_Declare(
        asio
        URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-12-2.tar.gz
)
FetchContent_MakeAvailable(asio)

FetchContent_GetProperties(websocket)
if(NOT websocket_POPULATED)
    FetchContent_Populate(websocket)
endif()

FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
endif()


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
        module/CTianshanWebSocketServer.cpp
        module/CTianshanWebSocketServer.h
)

target_compile_definitions(fileupload PRIVATE ASIO_STANDALONE _WEBSOCKETPP_CPP11_STL_)

target_include_directories(fileupload PRIVATE ${OpenCV_INCLUDE_DIRS} ${websocket_SOURCE_DIR} ${asio_SOURCE_DIR}/asio/include)
target_link_libraries(fileupload PUBLIC yaml-cpp::yaml-cpp nlohmann_json::nlohmann_json) # The library or executable that require yaml-cpp library
# Link your executable against the imported libcurl target
target_link_libraries(fileupload PRIVATE CURL::libcurl)
target_link_libraries(fileupload PRIVATE ${OpenCV_LIBS})
target_link_libraries(fileupload PRIVATE nadjieb_mjpeg_streamer::nadjieb_mjpeg_streamer)