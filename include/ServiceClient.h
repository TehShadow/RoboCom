#pragma once
class Node;  // Forward declare

#include "ServiceHeader.h"
#include "Serialization.h"
#include <unordered_map>
#include <functional>
#include <atomic>
#include <mutex>
#include <memory>
#include <string>
#include <iostream>

template<typename RequestT, typename ResponseT>
class ServiceClient {
public:
    using ResponseCallback = std::function<void(const ResponseT&)>;

    ServiceClient(std::shared_ptr<Node> node, const std::string& service_name);

    void call(const RequestT& request, ResponseCallback cb);

private:
    std::shared_ptr<Node> node_;
    std::string service_name_;
    std::atomic<uint64_t> request_counter_;

    std::unordered_map<uint64_t, ResponseCallback> pending_requests_;
    std::mutex mutex_;

    std::shared_ptr<class Subscriber> response_sub_;
    std::shared_ptr<class Publisher> request_pub_;
};

// 🚀 Include template implementation
#include "ServiceClient_impl.h"
