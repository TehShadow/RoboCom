#pragma once
#include "Transport.h"
#include "TopicPortMapper.h"
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

class PeerToPeerTcpTransport : public Transport {
public:
    PeerToPeerTcpTransport(const TransportConfig& config, const std::string& topic);
    ~PeerToPeerTcpTransport();

    void send(const std::string& topic, const std::string& msg, uint32_t seq_num, uint64_t timestamp) override;

    void set_receive_callback(ReceiveCallback cb) override {
        add_receive_callback(cb);
    }

    void add_receive_callback(ReceiveCallback cb);

    void add_peer(const std::string& peer_ip, uint16_t peer_port);

    uint16_t get_bound_port() const { return bound_port_; }

private:
    void server_loop();
    void client_recv_loop(int client_sock);

    int server_sock_;
    std::thread server_thread_;
    std::atomic<bool> running_;
    TransportConfig config_;
    std::string topic_;

    std::vector<int> peer_socks_;
    std::mutex peers_mutex_;

    std::vector<int> client_socks_;
    std::vector<std::thread> client_threads_;
    std::mutex clients_mutex_;

    std::vector<ReceiveCallback> callbacks_;
    std::mutex callbacks_mutex_;

    uint16_t bound_port_;
};