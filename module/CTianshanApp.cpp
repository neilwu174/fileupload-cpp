//
// Created by developer on 2025-12-26.
//

#include "CTianshanApp.h"

#include <future>
#include <netinet/in.h>

#include "CTianshanHtmlHomeHandler.h"
#include "CTianshanHtmlNotFoundHandler.h"
#include "CTianshanHttpController.h"
#include "CTianshanMultipartHandler.h"

static volatile std::sig_atomic_t g_shouldStop = 0;
static int g_listenFd = -1;

int CTianshanApp::run() {
    //Register Ctrl-C signal handler
    std::signal(SIGINT, [](int signal) {
        std::cout << "Caught SIGINT, shutting down...\n";
        ::g_shouldStop = 1;
        if (::g_listenFd >= 0) {
            /*
             *When :: is used without a namespace name in front of a method or variable,
             *it explicitly refers to the global scope or the global namespace. It can be used to access global variables.
             */
            ::close(g_listenFd);
        }
    });

    int port = config.getPort();
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
         * Handle http request in a detached thread to prevent the loop from blocking
         */
        std::thread([this, cfd] {
            CTianshanHttpController controller(config);
            controller.route("POST","/upload",[this](CTianshanHttpRequest& httpRequest)->CTianshanHttpResponse {
                auto handler = CTianshanMultipartHandler(config);
                return handler.accept(httpRequest);
            });
            controller.route("GET","/",[this](CTianshanHttpRequest& httpRequest)->CTianshanHttpResponse {
                auto handler = CTianshanHtmlHomeHandler(config);
                return handler.accept(httpRequest);
            });
            controller.route("GET","/home",[this](CTianshanHttpRequest& httpRequest)->CTianshanHttpResponse {
                auto handler = CTianshanHtmlHomeHandler(config);
                return handler.accept(httpRequest);
            });
            controller.route("GET","/not-found",[this](CTianshanHttpRequest& httpRequest)->CTianshanHttpResponse {
                auto handler = CTianshanHtmlNotFoundHandler(config);
                return handler.accept(httpRequest);
            });
            controller.proceed(cfd);
        }).detach();
    }

    return EXIT_SUCCESS;
}
