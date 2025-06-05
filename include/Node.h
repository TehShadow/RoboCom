#pragma once
#include "Publisher.h"
#include "Subscriber.h"
#include "ServiceClient.h"
#include "ServiceServer.h"
#include "TypedSubscriber.h"
#include "TypedServiceClient.h"
#include "TransportManager.h"
#include <memory>
#include <string>

/**
 * @brief Represents a node in the middleware graph.
 * 
 * Provides API to create publishers, subscribers, service clients, and service servers.
 */
class Node : public std::enable_shared_from_this<Node> {
public:
    // Constructor with external transport (optional)
    Node(const std::string& name, std::shared_ptr<Transport> transport)
        : name_(name), transport_(transport) {}

    // Constructor with TransportManager (preferred)
    Node(const std::string& name, const TransportConfig& config)
        : name_(name), transport_manager_(std::make_shared<TransportManager>(config)) {}

    std::shared_ptr<TransportManager> get_transport_manager() const {
        return transport_manager_;
    }

    const std::string& get_name() const {
        return name_;
    }

    // Publisher (raw)
    std::shared_ptr<Publisher> create_publisher(const std::string& topic) {
        auto transport = transport_manager_->get_transport(topic);
        return std::make_shared<Publisher>(topic, transport);
    }

    // Subscriber (raw buffer)
    std::shared_ptr<Subscriber> create_subscriber(const std::string& topic, Subscriber::SubscriberCallbackRaw callback) {
        auto transport = transport_manager_->get_transport(topic);
        return std::make_shared<Subscriber>(topic, transport, callback);
    }

    // TypedSubscriber (typed messages)
    template<typename T>
    std::shared_ptr<TypedSubscriber<T>> create_typed_subscriber(const std::string& topic) {
        return std::make_shared<TypedSubscriber<T>>(shared_from_this(), topic);
    }

    // ServiceClient (typed)
    template<typename RequestT, typename ResponseT>
    std::shared_ptr<ServiceClient<RequestT, ResponseT>> create_service_client(const std::string& service_name) {
        return std::make_shared<ServiceClient<RequestT, ResponseT>>(shared_from_this(), service_name);
    }

    // ServiceServer (typed)
    template<typename RequestT, typename ResponseT>
    std::shared_ptr<ServiceServer<RequestT, ResponseT>> create_service_server(
        const std::string& service_name,
        typename ServiceServer<RequestT, ResponseT>::HandlerCallback handler) 
    {
        return std::make_shared<ServiceServer<RequestT, ResponseT>>(shared_from_this(), service_name, handler);
    }

    // TypedServiceClient helper (optional wrapper for ServiceClient)
    template<typename RequestT, typename ResponseT>
    std::shared_ptr<TypedServiceClient<RequestT, ResponseT>> create_typed_service_client(const std::string& service_name) {
        return std::make_shared<TypedServiceClient<RequestT, ResponseT>>(shared_from_this(), service_name);
    }

private:
    std::string name_;

    // Optional: legacy transport
    std::shared_ptr<Transport> transport_;

    // Preferred: TransportManager
    std::shared_ptr<TransportManager> transport_manager_;
};
