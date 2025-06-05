#pragma once
#include "Subscriber.h"
#include "Serialization.h"
#include "Node.h"
#include <mutex>

/**
 * @brief Typed wrapper for Subscriber.
 */
template<typename T>
class TypedSubscriber {
public:
    TypedSubscriber(std::shared_ptr<Node> node, const std::string& topic) {
        sub_ = node->create_subscriber(topic, [this](uint32_t seq, uint64_t latency_us, const std::vector<uint8_t>& buffer) {
            T msg = deserialize<T>(buffer);
            {
                std::lock_guard<std::mutex> lock(mutex_);
                latest_msg_ = msg;
            }
        });
    }

    T get_latest() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return latest_msg_;
    }

private:
    std::shared_ptr<Subscriber> sub_;
    mutable std::mutex mutex_;
    T latest_msg_{};
};
