#pragma once
#include "ServiceServer.h"
#include "Serialization.h"
#include "PeerToPeerTcpTransport.h"
#include "ServiceHeader.h"  // 🚀 new include

template<typename RequestT, typename ResponseT>
ServiceServer<RequestT, ResponseT>::ServiceServer(std::shared_ptr<Node> node, const std::string& service_name, HandlerCallback handler)
    : node_(node), service_name_(service_name), handler_(handler)
{
    auto transport_manager = node_->get_transport_manager();

    auto req_transport = transport_manager->get_transport(service_name_ + "/request");
    auto resp_transport = transport_manager->get_transport(service_name_ + "/response");

    request_sub_ = std::make_shared<Subscriber>(service_name_ + "/request", req_transport,
        [this](uint32_t seq, uint64_t latency_us, const std::string& msg) {
            auto buffer = std::vector<uint8_t>(msg.begin(), msg.end());
            size_t offset = 0;
            auto request_header = deserialize<ServiceHeader>(buffer, offset);
            auto request = deserialize<RequestT>(buffer, offset);

            ResponseT response = handler_(request);

            ServiceHeader response_header{request_header.request_id};
            auto header_bytes = serialize(response_header);
            auto body_bytes = serialize(response);

            std::vector<uint8_t> full_resp;
            full_resp.insert(full_resp.end(), header_bytes.begin(), header_bytes.end());
            full_resp.insert(full_resp.end(), body_bytes.begin(), body_bytes.end());

            response_pub_->publish_raw(full_resp);

            std::cout << "[ServiceServer] Replied to request ID=" << request_header.request_id << std::endl;
        });

    response_pub_ = std::make_shared<Publisher>(service_name_ + "/response", resp_transport);
}