#pragma once
#include "Transport.h"
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <cstdint>
#include <chrono>

/**
 * @brief Subscriber for string or raw buffer messages.
 */
class Subscriber {
public:
    using SubscriberCallbackString = std::function<void(uint32_t, uint64_t, const std::string&)>;
    using SubscriberCallbackRaw    = std::function<void(uint32_t, uint64_t, const std::vector<uint8_t>&)>;

    // Constructor for legacy string-based subscriber
    Subscriber(const std::string& topic, std::shared_ptr<Transport> transport, SubscriberCallbackString callback)
        : topic_(topic), transport_(std::move(transport))
    {
        transport_->set_receive_callback(
            [callback](const std::string& topic, uint32_t seq, uint64_t timestamp, const std::string& msg) {
                uint64_t latency_us = current_time_us() - timestamp;
                callback(seq, latency_us, msg);
            }
        );
    }

    // Constructor for raw buffer subscriber
    Subscriber(const std::string& topic, std::shared_ptr<Transport> transport, SubscriberCallbackRaw callback)
        : topic_(topic), transport_(std::move(transport))
    {
        transport_->set_receive_callback(
            [callback](const std::string& topic, uint32_t seq, uint64_t timestamp, const std::string& msg) {
                std::vector<uint8_t> buffer(msg.begin(), msg.end());
                uint64_t latency_us = current_time_us() - timestamp;
                callback(seq, latency_us, buffer);
            }
        );
    }

    bool has_peers() const {
        return true; // Placeholder — real peer tracking coming later
    }

private:
    static uint64_t current_time_us() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    std::string topic_;
    std::shared_ptr<Transport> transport_;
};
