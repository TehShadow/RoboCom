#pragma once
#include "Publisher.h"
#include "Subscriber.h"
#include "ServiceHeader.h"  // 🚀 add this
#include <memory>
#include <string>
#include <mutex>
#include <optional>

class Node;

template<typename RequestT, typename ResponseT>
class ServiceClient {
public:
    ServiceClient(std::shared_ptr<Node> node, const std::string& service_name);

    void call(const RequestT& request, std::function<void(const ResponseT&)> user_cb);

    void call(const RequestT& request);

    bool wait_for_response(int timeout_ms = 1000);

    bool wait_for_service(int timeout_ms = 2000);

    ResponseT get_latest_response() const;

private:
    std::mutex callback_mutex_;
    std::function<void(const ResponseT&)> user_callback_;

    std::shared_ptr<Node> node_;
    std::string service_name_;

    std::shared_ptr<Publisher> request_pub_;
    std::shared_ptr<Subscriber> response_sub_;

    mutable std::mutex mutex_;
    std::optional<ResponseT> latest_resp_;
    uint32_t request_counter_;
    uint32_t latest_response_id_;  // 🚀 now used for matching
};

#include "ServiceClient_impl.h"
