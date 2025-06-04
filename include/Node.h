#pragma once
#include "Publisher.h"
#include "Subscriber.h"
#include "Discovery.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "PeerToPeerTcpTransport.h"

class Node {
public:
    Node(const std::string& name, std::shared_ptr<Transport> transport, std::shared_ptr<Discovery> discovery)
        : name_(name), transport_(transport), discovery_(discovery) 
    {
        // Advertise my port in Discovery!
        auto p2p = std::dynamic_pointer_cast<PeerToPeerTcpTransport>(transport_);
        if (p2p) {
            discovery_->set_local_port(p2p->get_bound_port());
            discovery_->set_peer_callback([p2p](const std::string& ip, uint16_t port) {
                p2p->add_peer(ip, port);
            });
            discovery_->start();
        }
    }

    ~Node() {
        if (discovery_) discovery_->stop();
    }

    std::shared_ptr<Publisher> create_publisher(const std::string& topic) {
        auto pub = std::make_shared<Publisher>(topic, transport_);
        publishers_[topic] = pub;
        return pub;
    }

    std::shared_ptr<Subscriber> create_subscriber(const std::string& topic, Subscriber::SubscriberCallbackRaw cb) {
        auto sub = std::make_shared<Subscriber>(topic, transport_, cb);
        subscribers_[topic].push_back(sub);
        return sub;
    }

private:
    std::string name_;
    std::shared_ptr<Transport> transport_;
    std::shared_ptr<Discovery> discovery_;

    std::unordered_map<std::string, std::shared_ptr<Publisher>> publishers_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Subscriber>>> subscribers_;
};