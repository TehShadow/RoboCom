#pragma once
#include "Publisher.h"
#include "Subscriber.h"
#include "ServiceHeader.h"  // 🚀 add this
#include <memory>
#include <string>
#include <mutex>
#include <functional>

template<typename RequestT, typename ResponseT>
class ServiceServer {
public:
    using HandlerCallback = std::function<ResponseT(const RequestT&)>;

    ServiceServer(std::shared_ptr<Node> node, const std::string& service_name, HandlerCallback handler);

private:
    std::shared_ptr<Node> node_;
    std::string service_name_;
    HandlerCallback handler_;

    std::shared_ptr<Subscriber> request_sub_;
    std::shared_ptr<Publisher> response_pub_;
};

#include "ServiceServer_impl.h"
