#pragma once
#include "Publisher.h"
#include "Subscriber.h"
#include "ServiceClient.h"
#include "ServiceServer.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "TypedSubscriber.h"
#include "TypedServiceClient.h"

class Node : public std::enable_shared_from_this<Node> {
public:
    Node(const std::string& name, std::shared_ptr<Transport> transport)
        : name_(name), transport_(transport) {}

    // Publisher
    std::shared_ptr<Publisher> create_publisher(const std::string& topic) {
        return std::make_shared<Publisher>(topic, transport_);
    }

    // String-based subscriber (old)
    std::shared_ptr<Subscriber> create_subscriber(const std::string& topic, Subscriber::SubscriberCallbackString callback) {
        return std::make_shared<Subscriber>(topic, transport_, callback);
    }

    // RAW buffer subscriber (new)
    std::shared_ptr<Subscriber> create_subscriber(const std::string& topic, Subscriber::SubscriberCallbackRaw callback) {
        return std::make_shared<Subscriber>(topic, transport_, callback);
    }

    template<typename T>
    std::shared_ptr<TypedSubscriber<T>> create_typed_subscriber(const std::string& topic) {
        return std::make_shared<TypedSubscriber<T>>(shared_from_this(), topic);
    }

    // ServiceClient helper
    template<typename RequestT, typename ResponseT>
    std::shared_ptr<ServiceClient<RequestT, ResponseT>> create_service_client(const std::string& service_name) {
        return std::make_shared<ServiceClient<RequestT, ResponseT>>(*this, service_name);
    }

    // ServiceServer helper
    template<typename RequestT, typename ResponseT>
    std::shared_ptr<ServiceServer<RequestT, ResponseT>> create_service_server(const std::string& service_name,
            typename ServiceServer<RequestT, ResponseT>::HandlerCallback handler) {
        return std::make_shared<ServiceServer<RequestT, ResponseT>>(*this, service_name, handler);
    }

    template<typename RequestT, typename ResponseT>
    std::shared_ptr<ServiceServer<RequestT, ResponseT>> create_typed_service_server(
        const std::string& service_name,
        typename ServiceServer<RequestT, ResponseT>::HandlerCallback handler)
    {
        return std::make_shared<ServiceServer<RequestT, ResponseT>>(*this, service_name, handler);
    }


private:
    std::string name_;
    std::shared_ptr<Transport> transport_;
};
