#pragma once
#include "Publisher.h"
#include "Subscriber.h"
#include "Transport.h"
#include <memory>
#include <string>

class Node {
public:
    Node(const std::string& name, std::shared_ptr<Transport> transport)
    : name_(name), transport_(std::move(transport)) {}

    std::shared_ptr<Publisher> create_publisher(const std::string& topic) {
        return std::make_shared<Publisher>(topic, transport_);
    }

    std::shared_ptr<Subscriber> create_subscriber(const std::string& topic, Subscriber::Callback cb) {
        return std::make_shared<Subscriber>(topic, cb, transport_);
    }

private:
    std::string name_;
    std::shared_ptr<Transport> transport_;
};