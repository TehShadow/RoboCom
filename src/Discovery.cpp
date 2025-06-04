#include "Discovery.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>

Discovery::Discovery(const std::string& topic, uint16_t discovery_port)
    : topic_(topic), port_(discovery_port), running_(false)
{}

Discovery::~Discovery() {
    stop();
}

void Discovery::start() {
    running_ = true;
    recv_thread_ = std::thread(&Discovery::recv_loop, this);
    send_thread_ = std::thread(&Discovery::send_loop, this);
}

void Discovery::stop() {
    running_ = false;
    if (recv_thread_.joinable()) recv_thread_.join();
    if (send_thread_.joinable()) send_thread_.join();
}

void Discovery::set_peer_callback(std::function<void(const std::string&, uint16_t)> cb) {
    peer_callback_ = cb;
}


void Discovery::recv_loop() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return;
    }

    ip_mreq mreq{};
    mreq.imr_multiaddr.s_addr = inet_addr("239.255.0.1");
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));

    char buffer[512];

    while (running_) {
        sockaddr_in src_addr{};
        socklen_t addrlen = sizeof(src_addr);
        ssize_t len = recvfrom(sock, buffer, sizeof(buffer)-1, 0, (sockaddr*)&src_addr, &addrlen);
        if (len < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[len] = '\0';
        std::string msg(buffer);
        // Expected format: "HELLO:<topic>:<port>"
        std::istringstream iss(msg);
        std::string token;

        std::getline(iss, token, ':');  // "HELLO"
        std::getline(iss, token, ':');  // topic
        std::string topic_in = token;

        std::getline(iss, token, ':');  // port
        uint16_t peer_port = std::stoi(token);

        if (topic_in == topic_) {
            std::string peer_ip = inet_ntoa(src_addr.sin_addr);

            std::lock_guard<std::mutex> lock(peers_mutex_);
            if (known_peers_.count(peer_ip + ":" + std::to_string(peer_port)) == 0) {
                known_peers_.insert(peer_ip + ":" + std::to_string(peer_port));
                std::cout << "[Discovery] New peer discovered: " << peer_ip << ":" << peer_port << std::endl;

                if (peer_callback_) {
                    // 🚀 Now pass both IP and PORT!
                    peer_callback_(peer_ip, peer_port);
                }
            }
        }
    }

    close(sock);
}

void Discovery::send_loop() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    int ttl = 1;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = inet_addr("239.255.0.1");

    std::string msg = "HELLO:" + topic_ + ":" + std::to_string(local_port_);

    while (running_) {
        ssize_t sent = sendto(sock, msg.c_str(), msg.size(), 0, (sockaddr*)&addr, sizeof(addr));
        if (sent < 0) {
            perror("sendto");
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));  // Send every 1 second
    }

    close(sock);
}
