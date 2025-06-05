#pragma once
#include "ServiceClient.h"
#include <mutex>
#include <optional>

template<typename RequestT, typename ResponseT>
class TypedServiceClient {
public:
    TypedServiceClient(std::shared_ptr<Node> node, const std::string& service_name)
        : client_(node, service_name) {}

    void call(const RequestT& request) {
        client_.call(request, [this](const ResponseT& resp) {
            std::lock_guard<std::mutex> lock(mutex_);
            latest_resp_ = resp;
        });
    }

    ResponseT get_latest_response() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return latest_resp_.value_or(ResponseT{});
    }

private:
    ServiceClient<RequestT, ResponseT> client_;
    mutable std::mutex mutex_;
    std::optional<ResponseT> latest_resp_;
};
