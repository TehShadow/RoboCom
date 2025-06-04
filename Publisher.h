#pragma once
#include "./TopicPortMapper.h"
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

class Publisher {
public:
    explicit Publisher(const std::string& topic)
    {
        port_ = map_topic_to_port(topic);
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0) {
            perror("socket");
            exit(1);
        }

        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port_);
        inet_pton(AF_INET, "127.0.0.1", &addr_.sin_addr);
    }

    void publish(const std::string& msg) {
        ssize_t sent = sendto(sock_, msg.c_str(), msg.size(), 0, (sockaddr*)&addr_, sizeof(addr_));
        if (sent < 0) perror("sendto");
    }

    ~Publisher() {
        close(sock_);
    }

private:
    int sock_;
    uint16_t port_;
    sockaddr_in addr_;
};
