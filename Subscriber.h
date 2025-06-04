#pragma once
#include "TopicPortMapper.h"
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

class Subscriber {
public:
    using Callback = std::function<void(const std::string&)>;

    Subscriber(const std::string& topic, Callback cb)
        : callback_(cb), running_(true)
    {
        port_ = map_topic_to_port(topic);
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0) {
            perror("socket");
            exit(1);
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind");
            exit(1);
        }

        thread_ = std::thread(&Subscriber::recv_loop, this);
    }

    ~Subscriber() {
        running_ = false;
        close(sock_);
        if (thread_.joinable())
            thread_.join();
    }

private:
    void recv_loop() {
        char buffer[1024];
        while (running_) {
            ssize_t len = recvfrom(sock_, buffer, sizeof(buffer)-1, 0, nullptr, nullptr);
            if (len < 0) {
                perror("recvfrom");
                continue;
            }
            buffer[len] = '\0';
            callback_(std::string(buffer));
        }
    }

    int sock_;
    uint16_t port_;
    Callback callback_;
    std::atomic<bool> running_;
    std::thread thread_;
};
