#pragma once
#include "Transport.h"
#include <string>
#include <memory>
#include <chrono>

class Publisher {
public:
    Publisher(const std::string& topic, std::shared_ptr<Transport> transport)
    : topic_(topic), transport_(transport), seq_num_(0) {}

    void publish(const std::string& msg) {
        uint64_t timestamp = current_time_us();
        transport_->send(topic_, msg, seq_num_++, timestamp);
    }

private:
    std::string topic_;
    std::shared_ptr<Transport> transport_;
    uint32_t seq_num_;

    static uint64_t current_time_us() {
        using namespace std::chrono;
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
};
