#ifndef FILEUPLOAD_CTIANSHANWEBSOCKETSERVER_H
#define FILEUPLOAD_CTIANSHANWEBSOCKETSERVER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>
#include <string>

typedef websocketpp::server<websocketpp::config::asio> server;

class CTianshanWebSocketServer {
public:
    CTianshanWebSocketServer(uint16_t port);
    ~CTianshanWebSocketServer();

    void start();
    void stop();

private:
    void on_open(websocketpp::connection_hdl hdl);
    void stream_mp4(websocketpp::connection_hdl hdl, std::string filepath);

    uint16_t m_port;
    server m_server;
    std::thread m_thread;
};

#endif //FILEUPLOAD_CTIANSHANWEBSOCKETSERVER_H
