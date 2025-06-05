#pragma once
#include "Transport.h"
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <cstdint>
#include <chrono>

class Subscriber {
public:
    // String-based callback (old)
    using SubscriberCallbackString = std::function<void(uint32_t, uint64_t, const std::string&)>;

    // NEW: Raw buffer callback (typed messages)
    using SubscriberCallbackRaw = std::function<void(uint32_t, uint64_t, const std::vector<uint8_t>&)>;

    // Constructor for string-based subscriber
    Subscriber(const std::string& topic, std::shared_ptr<Transport> transport, SubscriberCallbackString callback)
        : topic_(topic), transport_(transport)
    {
        transport_->set_receive_callback(
            [this, callback](const std::string& topic, uint32_t seq, uint64_t timestamp, const std::string& msg) {
                uint64_t latency_us = now() - timestamp;
                callback(seq, latency_us, msg);
            }
        );
    }

    // Constructor for raw buffer subscriber
    Subscriber(const std::string& topic, std::shared_ptr<Transport> transport, SubscriberCallbackRaw callback)
        : topic_(topic), transport_(transport)
    {
        transport_->set_receive_callback(
            [this, callback](const std::string& topic, uint32_t seq, uint64_t timestamp, const std::string& msg) {
                std::vector<uint8_t> buffer(msg.begin(), msg.end());
                uint64_t latency_us = now() - timestamp;
                callback(seq, latency_us, buffer);
            }
        );
    }

    bool has_peers() const {
    return true; // 🚀 Placeholder — will implement real count later
}

private:
    uint64_t now() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    std::string topic_;
    std::shared_ptr<Transport> transport_;
};
