#pragma once
#include "Transport.h"
#include "TopicPortMapper.h"
#include <thread>
#include <atomic>
#include <string>
#include <memory>
#include <mutex>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <iostream>

/**
 * @brief Simple UDP transport with optional multicast.
 *        Can be used for both publishers and subscribers.
 */
class UdpTransport : public Transport {
public:
    explicit UdpTransport(const TransportConfig& config, const std::string& bind_topic = "");
    ~UdpTransport();

    void send(const std::string& topic, const std::string& msg, uint32_t seq_num, uint64_t timestamp) override;

    void set_receive_callback(ReceiveCallback cb) override {
        callback_ = std::move(cb);
    }

private:
    void recv_loop();

    int sock_;
    std::thread thread_;
    std::atomic<bool> running_;
    ReceiveCallback callback_;
    TransportConfig config_;
};
