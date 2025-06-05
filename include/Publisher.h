#pragma once
#include "Transport.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

class Publisher {
public:
    Publisher(const std::string& topic, std::shared_ptr<Transport> transport)
        : topic_(topic), transport_(transport), sequence_number_(0) {}

    // OLD (string-based, keep for compatibility)
    void publish(const std::string& msg) {
        uint64_t timestamp = now();
        transport_->send(topic_, msg, sequence_number_++, timestamp);
    }

    // NEW (typed messages)
    void publish_raw(const std::vector<uint8_t>& buffer) {
        uint64_t timestamp = now();
        transport_->send(topic_, std::string(buffer.begin(), buffer.end()), sequence_number_++, timestamp);
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
    uint32_t sequence_number_;
};

