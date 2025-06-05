#include "UdpTransport.h"

UdpTransport::UdpTransport(const TransportConfig& config, const std::string& bind_topic)
    : running_(true), config_(config)
{
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ < 0) {
        perror("socket");
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
        perror("setsockopt: SO_REUSEADDR");
        exit(1);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;

    if (bind_topic.empty()) {
        // Publisher mode → any port
        addr.sin_port = htons(0);
        std::cout << "[UdpTransport] Starting in publisher mode." << std::endl;
    } else {
        // Subscriber mode → bind to topic port
        uint16_t port = map_topic_to_port(bind_topic);
        addr.sin_port = htons(port);
        std::cout << "[UdpTransport] Binding to port " << port << " for topic " << bind_topic << std::endl;
    }

    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // Set TTL
    int ttl = 16;
    if (setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt: IP_MULTICAST_TTL");
    }

    // Multicast if requested
    if (config_.use_multicast && config_.iface_ip != "127.0.0.1") {
        ip_mreq mreq{};
        inet_pton(AF_INET, config_.multicast_addr.c_str(), &mreq.imr_multiaddr);
        inet_pton(AF_INET, config_.iface_ip.c_str(), &mreq.imr_interface);

        if (setsockopt(sock_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
            perror("setsockopt: IP_ADD_MEMBERSHIP");
            exit(1);
        }

        std::cout << "[UdpTransport] Joined multicast group on iface " << config_.iface_ip << std::endl;
    } else {
        std::cout << "[UdpTransport] Using unicast on iface " << config_.iface_ip << std::endl;
    }

    std::cout << "[UdpTransport] recv_loop running" << std::endl;
    thread_ = std::thread(&UdpTransport::recv_loop, this);
}

UdpTransport::~UdpTransport() {
    running_ = false;
    close(sock_);
    if (thread_.joinable()) thread_.join();
}

void UdpTransport::send(const std::string& topic, const std::string& msg, uint32_t seq_num, uint64_t timestamp) {
    uint16_t port = map_topic_to_port(topic);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (config_.use_multicast && config_.iface_ip != "127.0.0.1") {
        inet_pton(AF_INET, config_.multicast_addr.c_str(), &addr.sin_addr);
    } else {
        inet_pton(AF_INET, config_.iface_ip.c_str(), &addr.sin_addr);
    }

    char buffer[1500];
    size_t offset = 0;

    std::memcpy(buffer + offset, &seq_num, sizeof(seq_num));
    offset += sizeof(seq_num);

    std::memcpy(buffer + offset, &timestamp, sizeof(timestamp));
    offset += sizeof(timestamp);

    std::memcpy(buffer + offset, msg.data(), msg.size());
    offset += msg.size();

    ssize_t sent = sendto(sock_, buffer, offset, 0, (sockaddr*)&addr, sizeof(addr));
    if (sent < 0) {
        perror("sendto");
    } else {
        std::cout << "[UdpTransport] Sent seq=" << seq_num << " to "
                  << (config_.use_multicast ? config_.multicast_addr : config_.iface_ip)
                  << " port=" << port << std::endl;
    }
}

void UdpTransport::recv_loop() {
    char buffer[1500];

    while (running_) {
        sockaddr_in src_addr{};
        socklen_t addrlen = sizeof(src_addr);

        ssize_t len = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&src_addr, &addrlen);
        if (len < 0) {
            perror("recvfrom");
            continue;
        }

        size_t offset = 0;

        uint32_t seq;
        uint64_t timestamp;

        std::memcpy(&seq, buffer + offset, sizeof(seq));
        offset += sizeof(seq);

        std::memcpy(&timestamp, buffer + offset, sizeof(timestamp));
        offset += sizeof(timestamp);

        std::string payload(buffer + offset, len - offset);

        std::string topic = "[port_" + std::to_string(ntohs(src_addr.sin_port)) + "]";

        if (callback_) {
            callback_(topic, seq, timestamp, payload);
        }
    }
}
