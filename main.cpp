#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <cerrno>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <future>

#include "module/CTianshanApp.h"
#include "module/CTianshanConfig.h"
#include "module/CTianshanFilesystem.h"
#include "module/CTianshanHttpController.h"
#include "module/CTianshanMultipartHandler.h"
#include "module/CTianshanHtmlHomeHandler.h"
#include "module/inja.h"

#include "module/file_utils.h"
namespace fs = std::filesystem;

void test() {
    // std::string sourcePath = "/Users/developer/Downloads/encrypt/video-encrypt.mp4";
    // std::string destPath = "/Users/developer/Downloads/encrypt/video-1.mp4";
    // std::string key = "meiyou";
    // cryptFile(sourcePath, destPath, key);

    std::string path = "/Users/developer/Downloads/encrypt";
    std::filesystem::path dest = "/Users/developer/Downloads/encrypt-complete";
    if (!exists(dest)) {
        std::filesystem::create_directories(dest);
    }
    std::vector<TianshanFile> files = scan_directory(path);
    encryptRecursively(files,"meiyou");
}
int main(int argc, const char * argv[]) {
    test();
    // CTianshanConfig *httpConfig = new CTianshanConfig();
    // httpConfig->load_config();
    // CTianshanApp app(*httpConfig);
    // app.run();
    // std::cerr << "Shutting down.\n";
    // delete httpConfig;
    return EXIT_SUCCESS;
}
