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

add_executable(fileupload main.cpp
        module/CUploadService.cpp
        module/CUploadService.h
        module/CTianshanHttp.cpp
        module/CTianshanHttp.h
)
target_link_libraries(fileupload PUBLIC yaml-cpp::yaml-cpp) # The library or executable that require yaml-cpp library
