#pragma once
#include "Node.h"  // 🚀 Now safe — no circular dependency!

template<typename RequestT, typename ResponseT>
ServiceServer<RequestT, ResponseT>::ServiceServer(Node& node, const std::string& service_name, HandlerCallback handler)
    : node_(node), service_name_(service_name), handler_(handler)
{
    request_sub_ = node_.create_subscriber(service_name_ + "/request",
        [this](uint32_t seq, uint64_t latency_us, const std::vector<uint8_t>& buffer) {
            ServiceHeader header;
            std::memcpy(&header, buffer.data(), sizeof(header));

            {
                std::lock_guard<std::mutex> lock(proc_mutex_);
                if (processed_requests_.count(header.request_id) > 0) {
                    std::cout << "[ServiceServer] Duplicate request_id=" << header.request_id << " → IGNORE" << std::endl;
                    return;
                }
                processed_requests_.insert(header.request_id);
            }

            RequestT request = deserialize<RequestT>(
                std::vector<uint8_t>(buffer.begin() + sizeof(header), buffer.end()));

            std::cout << "[ServiceServer] Processing request_id=" << header.request_id << std::endl;

            ResponseT response = handler_(request);

            ServiceHeader response_header{ header.request_id };
            auto resp_buffer = serialize(response);

            std::vector<uint8_t> out_buffer(sizeof(response_header) + resp_buffer.size());
            std::memcpy(out_buffer.data(), &response_header, sizeof(response_header));
            std::memcpy(out_buffer.data() + sizeof(response_header), resp_buffer.data(), resp_buffer.size());

            if (!response_pub_) {
                response_pub_ = node_.create_publisher(service_name_ + "/response");
            }

            response_pub_->publish_raw(out_buffer);

            std::cout << "[ServiceServer] Sent response for request_id=" << header.request_id << std::endl;
        });
}
