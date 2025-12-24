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

#include "module/CTianshanConfig.h"
#include "module/CTianshanHttpController.h"
#include "module/CTianshanMultipartHandler.h"

namespace fs = std::filesystem;

static volatile std::sig_atomic_t g_shouldStop = 0;
static int g_listenFd = -1;

void handleSigInt(int) {
    g_shouldStop = 1;
    if (g_listenFd >= 0) {
        ::close(g_listenFd);
    }
}

int main(int argc, const char * argv[]) {
    std::signal(SIGINT, handleSigInt);

    CTianshanConfig *httpConfig = new CTianshanConfig();
    httpConfig->load_config();

    int port = httpConfig->getPort();
    g_listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (g_listenFd < 0) {
        std::cerr << "Failed to create socket: " << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }

    int yes = 1;
    ::setsockopt(g_listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (::bind(g_listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Bind failed: " << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }

    if (::listen(g_listenFd, 16) < 0) {
        std::cerr << "Listen failed: " << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }

    std::cerr << "HTTP server listening on http://0.0.0.0:" << port << "\n";

    while (!g_shouldStop) {
        sockaddr_in cli{}; socklen_t clilen = sizeof(cli);
        int cfd = ::accept(g_listenFd, reinterpret_cast<sockaddr*>(&cli), &clilen);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) { continue; }
            // transient error
            continue;
        }
        /*
         * Handle http request in an asyc mode
         */
        std::future<void> f = std::async(std::launch::async, [=] {
            CTianshanHttpController controller;
            controller.route("POST","/upload",*httpConfig,[httpConfig](CTianshanHttpRequest& httpRequest)->CTianshanHttpResponse {
                CTianshanMultipartHandler handler = CTianshanMultipartHandler(httpConfig->getUploadFolder());
                return handler.accept(httpRequest);
            });
            controller.proceed(cfd,*httpConfig);
        });
    }

    std::cerr << "Shutting down.\n";
    delete httpConfig;
    return EXIT_SUCCESS;
}
