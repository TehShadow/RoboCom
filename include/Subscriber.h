#pragma once
#include "Transport.h"
#include <string>
#include <memory>
#include <chrono>

class Subscriber {
public:
    using Callback = std::function<void(uint32_t, uint64_t, const std::string&)>;

    Subscriber(const std::string& topic, Callback cb, std::shared_ptr<Transport> transport)
    : topic_(topic), callback_(cb), transport_(transport) {
        transport_->set_receive_callback([this](const std::string& topic, uint32_t seq, uint64_t timestamp, const std::string& msg) {
            // You can add topic filter here!
            this->callback_(seq, current_time_us() - timestamp, msg);
        });
    }

private:
    std::string topic_;
    Callback callback_;
    std::shared_ptr<Transport> transport_;

    static uint64_t current_time_us() {
        using namespace std::chrono;
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
};
