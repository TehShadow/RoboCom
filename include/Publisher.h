#pragma once
#include "Transport.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

/**
 * @brief Simple Publisher for raw or string-based messages.
 */
class Publisher {
public:
    Publisher(const std::string& topic, std::shared_ptr<Transport> transport)
        : topic_(topic), transport_(std::move(transport)), sequence_number_(0) {}

    // Legacy string-based publish
    void publish(const std::string& msg) {
        uint64_t timestamp = current_time_us();
        transport_->send(topic_, msg, sequence_number_++, timestamp);
    }

    // Typed (raw buffer) publish
    void publish_raw(const std::vector<uint8_t>& buffer) {
        uint64_t timestamp = current_time_us();
        transport_->send(topic_, std::string(buffer.begin(), buffer.end()), sequence_number_++, timestamp);
    }

    // Placeholder — will implement peer tracking later
    bool has_peers() const {
        return true;
    }

private:
    uint64_t current_time_us() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    std::string topic_;
    std::shared_ptr<Transport> transport_;
    uint32_t sequence_number_;
};
