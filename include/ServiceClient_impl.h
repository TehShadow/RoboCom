#pragma once
#include "ServiceClient.h"
#include "Serialization.h"
#include "PeerToPeerTcpTransport.h"
#include "ServiceHeader.h" // 🚀 new include


template<typename RequestT, typename ResponseT>
void ServiceClient<RequestT, ResponseT>::call(const RequestT& request, std::function<void(const ResponseT&)> user_cb) {
    uint32_t req_id;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        latest_resp_.reset();
        req_id = request_counter_++;
        latest_response_id_ = req_id;  // 🚀 store for matching
    }

    {
        std::lock_guard<std::mutex> lock(callback_mutex_);
        user_callback_ = user_cb;
    }

    // 🚀 Compose message: header + body
    ServiceHeader header{req_id};
    auto header_bytes = serialize(header);
    auto body_bytes = serialize(request);

    std::vector<uint8_t> full_msg;
    full_msg.insert(full_msg.end(), header_bytes.begin(), header_bytes.end());
    full_msg.insert(full_msg.end(), body_bytes.begin(), body_bytes.end());

    request_pub_->publish_raw(full_msg);

    std::cout << "[ServiceClient] Sent request ID=" << req_id << std::endl;
}

template<typename RequestT, typename ResponseT>
void ServiceClient<RequestT, ResponseT>::call(const RequestT& request) {
    uint32_t req_id;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        latest_resp_.reset();
        req_id = request_counter_++;
        latest_response_id_ = req_id;  // 🚀 store for matching
    }

    // 🚀 Compose message: header + body
    ServiceHeader header{req_id};
    auto header_bytes = serialize(header);
    auto body_bytes = serialize(request);

    std::vector<uint8_t> full_msg;
    full_msg.insert(full_msg.end(), header_bytes.begin(), header_bytes.end());
    full_msg.insert(full_msg.end(), body_bytes.begin(), body_bytes.end());

    request_pub_->publish_raw(full_msg);

    std::cout << "[ServiceClient] Sent request ID=" << req_id << std::endl;
}

template<typename RequestT, typename ResponseT>
ServiceClient<RequestT, ResponseT>::ServiceClient(std::shared_ptr<Node> node, const std::string& service_name)
    : node_(node), service_name_(service_name), request_counter_(1), latest_response_id_(0)
{
    auto transport_manager = node_->get_transport_manager();

    auto req_transport = transport_manager->get_transport(service_name_ + "/request");
    auto resp_transport = transport_manager->get_transport(service_name_ + "/response");

    request_pub_ = std::make_shared<Publisher>(service_name_ + "/request", req_transport);

    response_sub_ = std::make_shared<Subscriber>(service_name_ + "/response", resp_transport,
        [this](uint32_t seq, uint64_t latency_us, const std::string& msg) {
            auto buffer = std::vector<uint8_t>(msg.begin(), msg.end());
            size_t offset = 0;
            auto response_header = deserialize<ServiceHeader>(buffer, offset);
            auto response = deserialize<ResponseT>(buffer, offset);

            bool match = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (response_header.request_id == latest_response_id_) {
                    latest_resp_ = response;
                    match = true;
                }
            }

            std::cout << "[ServiceClient] Received response ID=" << response_header.request_id 
                      << " latency=" << latency_us << " us"
                      << (match ? " [MATCH]" : " [IGNORED]") << std::endl;

            std::function<void(const ResponseT&)> local_cb;
            {
                std::lock_guard<std::mutex> lock(callback_mutex_);
                local_cb = user_callback_;
            }
            if (match && local_cb) {
                local_cb(response);
            }
        });
}


template<typename RequestT, typename ResponseT>
bool ServiceClient<RequestT, ResponseT>::wait_for_response(int timeout_ms) {
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

template<typename RequestT, typename ResponseT>
bool ServiceClient<RequestT, ResponseT>::wait_for_service(int timeout_ms) {
    using namespace std::chrono;
    auto start = steady_clock::now();

    while (true) {
        if (request_pub_->has_peers() && response_sub_->has_peers()) {
            return true;
        }

        if (duration_cast<milliseconds>(steady_clock::now() - start).count() > timeout_ms) {
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

template<typename RequestT, typename ResponseT>
ResponseT ServiceClient<RequestT, ResponseT>::get_latest_response() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return latest_resp_.value_or(ResponseT{});
}
