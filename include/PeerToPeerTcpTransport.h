#pragma once
#include "Transport.h"
#include "TopicPortMapper.h"
#include "Discovery.h"
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <memory>

/**
 * @brief TCP-based transport with peer-to-peer connections and multicast discovery.
 */
class PeerToPeerTcpTransport : public Transport {
public:
    PeerToPeerTcpTransport(const TransportConfig& config, const std::string& topic);
    ~PeerToPeerTcpTransport();

    void send(const std::string& topic, const std::string& msg, uint32_t seq_num, uint64_t timestamp) override;

    void set_receive_callback(ReceiveCallback cb) override {
        callback_ = cb;
    }

    void add_peer(const std::string& peer_ip, uint16_t peer_port);

    uint16_t get_listen_port() const {
        return config_.dynamic_port;
    }

    std::shared_ptr<Discovery> get_discovery() const {
        return discovery_;
    }

private:
    void server_loop();
    void client_recv_loop(int client_sock);

    int server_sock_;
    std::thread server_thread_;
    std::atomic<bool> running_;
    ReceiveCallback callback_;
    TransportConfig config_;
    std::string topic_;

    std::vector<int> peer_socks_;
    std::mutex peers_mutex_;

    std::vector<int> client_socks_;
    std::vector<std::thread> client_threads_;
    std::mutex clients_mutex_;

    std::shared_ptr<Discovery> discovery_;
};
