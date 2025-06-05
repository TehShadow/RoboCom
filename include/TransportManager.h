#pragma once
#include "PeerToPeerTcpTransport.h"
#include <unordered_map>
#include <memory>
#include <mutex>

class TransportManager {
public:
    TransportManager(const TransportConfig& config)
        : config_(config) {}

    std::shared_ptr<PeerToPeerTcpTransport> get_transport(const std::string& topic) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = transports_.find(topic);
        if (it != transports_.end()) {
            return it->second;
        }

        auto transport = std::make_shared<PeerToPeerTcpTransport>(config_, topic);
        transport->get_discovery()->start();

        transports_[topic] = transport;
        return transport;
    }

private:
    TransportConfig config_;
    std::unordered_map<std::string, std::shared_ptr<PeerToPeerTcpTransport>> transports_;
    std::mutex mutex_;
};
