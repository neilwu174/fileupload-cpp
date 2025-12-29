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

namespace fs = std::filesystem;

void test() {
    CTianshanFilesystem filesystem;
    std::string path = "/Users/developer/Downloads";
    filesystem.scan(path);
}
int main(int argc, const char * argv[]) {
    test();
    CTianshanConfig *httpConfig = new CTianshanConfig();
    httpConfig->load_config();
    CTianshanApp app(*httpConfig);
    app.run();
    std::cerr << "Shutting down.\n";
    delete httpConfig;
    return EXIT_SUCCESS;
}
