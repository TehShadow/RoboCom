#pragma once
#include "ServiceClient.h"
#include <mutex>
#include <optional>
#include <chrono>

/**
 * @brief Typed wrapper for ServiceClient.
 */
template<typename RequestT, typename ResponseT>
class TypedServiceClient {
public:
    TypedServiceClient(std::shared_ptr<Node> node, const std::string& service_name)
        : client_(node, service_name) {}

    void call(const RequestT& request) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            latest_resp_.reset();
        }

        client_.call(request, [this](const ResponseT& resp) {
            std::lock_guard<std::mutex> lock(mutex_);
            latest_resp_ = resp;
        });
    }

    bool wait_for_response(int timeout_ms = 1000) {
        using namespace std::chrono;
        auto start = steady_clock::now();

        while (true) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (latest_resp_.has_value()) {
                    return true;
                }
            }

            if (duration_cast<milliseconds>(steady_clock::now() - start).count() > timeout_ms) {
                return false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
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
