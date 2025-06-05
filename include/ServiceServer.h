#pragma once
class Node;  // 🚀 Forward declare

#include "ServiceHeader.h"
#include "Serialization.h"
#include <unordered_set>
#include <functional>
#include <string>
#include <memory>
#include <mutex>
#include <iostream>

template<typename RequestT, typename ResponseT>
class ServiceServer {
public:
    using HandlerCallback = std::function<ResponseT(const RequestT&)>;

    ServiceServer(Node& node, const std::string& service_name, HandlerCallback handler);

private:
    Node& node_;
    std::string service_name_;
    HandlerCallback handler_;
    std::shared_ptr<class Subscriber> request_sub_;
    std::shared_ptr<class Publisher> response_pub_;

    std::unordered_set<uint64_t> processed_requests_;
    std::mutex proc_mutex_;
};

// 🚀 Include the implementation template
#include "ServiceServer_impl.h"