#include "CTianshanWebSocketServer.h"
#include <fstream>
#include <iostream>

CTianshanWebSocketServer::CTianshanWebSocketServer(uint16_t port) : m_port(port) {
    m_server.init_asio();
    m_server.set_open_handler(bind(&CTianshanWebSocketServer::on_open, this, std::placeholders::_1));
    m_server.clear_access_channels(websocketpp::log::alevel::all);
}

CTianshanWebSocketServer::~CTianshanWebSocketServer() {
    stop();
}

void CTianshanWebSocketServer::start() {
    m_thread = std::thread([this]() {
        m_server.listen(m_port);
        m_server.start_accept();
        std::cout << "WebSocket server listening on port " << m_port << std::endl;
        m_server.run();
    });
}

void CTianshanWebSocketServer::stop() {
    m_server.stop_listening();
    // In a real application, you'd want to close all connections here.
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void CTianshanWebSocketServer::on_open(websocketpp::connection_hdl hdl) {
    std::cout << "WebSocket connection opened" << std::endl;
    // For demonstration, we stream a fixed mp4 file if it exists.
    // In a real app, this might be triggered by a message.
    std::string filepath = "test.mp4"; 
    std::thread(&CTianshanWebSocketServer::stream_mp4, this, hdl, filepath).detach();
}

void CTianshanWebSocketServer::stream_mp4(websocketpp::connection_hdl hdl, std::string filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open MP4 file: " << filepath << std::endl;
        return;
    }

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        websocketpp::lib::error_code ec;
        m_server.send(hdl, buffer, file.gcount(), websocketpp::frame::opcode::binary, ec);
        if (ec) {
            std::cerr << "WebSocket send failed: " << ec.message() << std::endl;
            break;
        }
        // Small delay to prevent overwhelming the client/buffer for this simple example
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "Streaming finished" << std::endl;
}
