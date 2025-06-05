#pragma once
#include "Node.h"  // Safe now — no circular dependency

template<typename RequestT, typename ResponseT>
ServiceClient<RequestT, ResponseT>::ServiceClient(std::shared_ptr<Node> node, const std::string& service_name)
    : node_(node), service_name_(service_name), request_counter_(1)
{
    response_sub_ = node_->create_subscriber(service_name_ + "/response",
        [this](uint32_t seq, uint64_t latency_us, const std::vector<uint8_t>& buffer) {
            ServiceHeader header;
            std::memcpy(&header, buffer.data(), sizeof(header));

            ResponseT response = deserialize<ResponseT>(
                std::vector<uint8_t>(buffer.begin() + sizeof(header), buffer.end()));

            std::lock_guard<std::mutex> lock(mutex_);
            auto it = pending_requests_.find(header.request_id);
            if (it != pending_requests_.end()) {
                it->second(response);
                pending_requests_.erase(it);
            } else {
                std::cout << "[ServiceClient] Unknown response ID=" << header.request_id << " → IGNORE" << std::endl;
            }
        });

    request_pub_ = node_->create_publisher(service_name_ + "/request");
}

template<typename RequestT, typename ResponseT>
void ServiceClient<RequestT, ResponseT>::call(const RequestT& request, ResponseCallback cb) {
    uint64_t request_id = request_counter_++;

    ServiceHeader header{ request_id };
    auto req_buffer = serialize(request);

    std::vector<uint8_t> buffer(sizeof(header) + req_buffer.size());
    std::memcpy(buffer.data(), &header, sizeof(header));
    std::memcpy(buffer.data() + sizeof(header), req_buffer.data(), req_buffer.size());

    {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_requests_[request_id] = cb;
    }

    request_pub_->publish_raw(buffer);

    std::cout << "[ServiceClient] Sent request ID=" << request_id << std::endl;
}
