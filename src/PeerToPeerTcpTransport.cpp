#include "PeerToPeerTcpTransport.h"
#include <cstring>
#include <chrono>

PeerToPeerTcpTransport::PeerToPeerTcpTransport(const TransportConfig& config, const std::string& topic)
    : running_(true), config_(config), topic_(topic)
{
    server_sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_ < 0) {
        perror("socket");
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(server_sock_, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
        perror("setsockopt: SO_REUSEADDR");
        exit(1);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0); // OS picks port

    if (bind(server_sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    socklen_t addrlen = sizeof(addr);
    getsockname(server_sock_, (sockaddr*)&addr, &addrlen);
    uint16_t actual_port = ntohs(addr.sin_port);

    std::cout << "[PeerToPeerTcpTransport] Bound to port " << actual_port << " for topic " << topic_ << std::endl;

    config_.dynamic_port = actual_port;

    if (listen(server_sock_, 10) < 0) {
        perror("listen");
        exit(1);
    }

    std::cout << "[PeerToPeerTcpTransport] Listening on port " << actual_port << " for topic " << topic_ << std::endl;

    // Create Discovery
    discovery_ = std::make_shared<Discovery>(topic_);
    discovery_->set_local_port(get_listen_port());

    discovery_->set_peer_callback([this](const std::string& ip, uint16_t port) {
        std::cout << "[Discovery] New peer: " << ip << ":" << port << std::endl;
        this->add_peer(ip, port);
    });

    server_thread_ = std::thread(&PeerToPeerTcpTransport::server_loop, this);
}

PeerToPeerTcpTransport::~PeerToPeerTcpTransport() {
    running_ = false;
    close(server_sock_);

    if (server_thread_.joinable()) server_thread_.join();

    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        for (auto sock : peer_socks_) {
            close(sock);
        }
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (auto sock : client_socks_) {
            close(sock);
        }
        for (auto& t : client_threads_) {
            if (t.joinable()) t.join();
        }
    }
}

void PeerToPeerTcpTransport::send(const std::string& topic, const std::string& msg, uint32_t seq_num, uint64_t timestamp) {
    (void)topic;

    char buffer[1500];
    size_t offset = 0;

    std::memcpy(buffer + offset, &seq_num, sizeof(seq_num));
    offset += sizeof(seq_num);

    std::memcpy(buffer + offset, &timestamp, sizeof(timestamp));
    offset += sizeof(timestamp);

    std::memcpy(buffer + offset, msg.data(), msg.size());
    offset += msg.size();

    std::lock_guard<std::mutex> lock(peers_mutex_);
    for (auto sock : peer_socks_) {
        ssize_t sent = write(sock, buffer, offset);
        if (sent < 0) {
            perror("write (peer)");
        } else {
            std::cout << "[PeerToPeerTcpTransport] Sent to peer sock=" << sock << " SEQ=" << seq_num << std::endl;
        }
    }
}

void PeerToPeerTcpTransport::add_peer(const std::string& peer_ip, uint16_t peer_port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(peer_port);
    inet_pton(AF_INET, peer_ip.c_str(), &addr.sin_addr);

    std::cout << "[PeerToPeerTcpTransport] Connecting to peer " << peer_ip << ":" << peer_port << std::endl;

    while (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect (retrying)");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "[PeerToPeerTcpTransport] Connected to peer " << peer_ip << std::endl;

    std::lock_guard<std::mutex> lock(peers_mutex_);
    peer_socks_.push_back(sock);
}

void PeerToPeerTcpTransport::server_loop() {
    while (running_) {
        sockaddr_in client_addr{};
        socklen_t addrlen = sizeof(client_addr);

        std::cout << "[PeerToPeerTcpTransport] Waiting for incoming peer connection..." << std::endl;

        int client_sock = accept(server_sock_, (sockaddr*)&client_addr, &addrlen);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        std::cout << "[PeerToPeerTcpTransport] Peer connected: sock=" << client_sock << std::endl;

        {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            client_socks_.push_back(client_sock);
            client_threads_.emplace_back(&PeerToPeerTcpTransport::client_recv_loop, this, client_sock);
        }
    }
}

void PeerToPeerTcpTransport::client_recv_loop(int client_sock) {
    char buffer[1500];

    while (running_) {
        ssize_t len = read(client_sock, buffer, sizeof(buffer) - 1);
        if (len < 0) {
            perror("read");
            break;
        } else if (len == 0) {
            std::cout << "[PeerToPeerTcpTransport] Peer disconnected: sock=" << client_sock << std::endl;
            break;
        }

        size_t offset = 0;

        uint32_t seq;
        uint64_t timestamp;

        std::memcpy(&seq, buffer + offset, sizeof(seq));
        offset += sizeof(seq);

        std::memcpy(&timestamp, buffer + offset, sizeof(timestamp));
        offset += sizeof(timestamp);

        std::string payload(buffer + offset, len - offset);

        if (callback_) {
            callback_(topic_, seq, timestamp, payload);
        }
    }

    close(client_sock);
}
