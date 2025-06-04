#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_set>

class Discovery {
public:
    Discovery(const std::string& topic, uint16_t discovery_port = 9999);
    ~Discovery();

    void start();
    void stop();

    void set_peer_callback(std::function<void(const std::string&, uint16_t)> cb);

    void set_local_port(uint16_t port) {
    local_port_ = port;
}

private:
    void recv_loop();
    void send_loop();

    std::string topic_;
    uint16_t port_;
    std::atomic<bool> running_;
    std::thread recv_thread_;
    std::thread send_thread_;
    std::function<void(const std::string&, uint16_t)> peer_callback_;
    uint16_t local_port_ = 0;
    std::unordered_set<std::string> known_peers_;
    std::mutex peers_mutex_;
};
