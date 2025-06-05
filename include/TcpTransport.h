#pragma once
#include "Transport.h"
#include "TopicPortMapper.h"
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <iostream>

/**
 * @brief Simple TCP transport with server and client modes.
 *        Supports multiple subscribers and server-side broadcasts.
 */
class TcpTransport : public Transport {
public:
    TcpTransport(const TransportConfig& config, const std::string& topic, bool is_server);
    ~TcpTransport();

    void send(const std::string& topic, const std::string& msg, uint32_t seq_num, uint64_t timestamp) override;

    void set_receive_callback(ReceiveCallback cb) override {
        add_receive_callback(std::move(cb));
    }

    void add_receive_callback(ReceiveCallback cb);

private:
    void server_loop();
    void client_recv_loop(int client_sock);

    int sock_;
    std::thread thread_;
    std::atomic<bool> running_;
    TransportConfig config_;
    std::string topic_;
    bool is_server_;

    std::vector<std::thread> client_threads_;
    std::vector<int> client_socks_;
    std::mutex clients_mutex_;

    std::vector<ReceiveCallback> callbacks_;
    std::mutex callbacks_mutex_;
};
