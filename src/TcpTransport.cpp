#include "TcpTransport.h"

TcpTransport::TcpTransport(const TransportConfig& config, const std::string& topic, bool is_server)
    : running_(true), config_(config), topic_(topic), is_server_(is_server)
{
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        perror("socket");
        exit(1);
    }

    if (is_server_) {
        int reuse = 1;
        if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
            perror("setsockopt: SO_REUSEADDR");
            exit(1);
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        uint16_t port = map_topic_to_port(topic_);
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind");
            exit(1);
        }

        if (listen(sock_, 10) < 0) {
            perror("listen");
            exit(1);
        }

        std::cout << "[TcpTransport] Listening on port " << port << " for topic " << topic_ << std::endl;

        thread_ = std::thread(&TcpTransport::server_loop, this);
    }
    else {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        uint16_t port = map_topic_to_port(topic_);
        addr.sin_port = htons(port);
        inet_pton(AF_INET, config_.iface_ip.c_str(), &addr.sin_addr);

        std::cout << "[TcpTransport] Connecting to " << config_.iface_ip << " port " << port << std::endl;

        while (connect(sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("connect (retrying)");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        std::cout << "[TcpTransport] Connected!" << std::endl;
    }
}

TcpTransport::~TcpTransport() {
    running_ = false;
    close(sock_);
    if (thread_.joinable()) thread_.join();

    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto& t : client_threads_) {
        if (t.joinable()) t.join();
    }
    for (auto csock : client_socks_) {
        close(csock);
    }
}

void TcpTransport::send(const std::string& topic, const std::string& msg, uint32_t seq_num, uint64_t timestamp) {
    if (is_server_) {
        std::cerr << "[TcpTransport] ERROR: server cannot send()" << std::endl;
        return;
    }

    char buffer[1500];
    size_t offset = 0;

    std::memcpy(buffer + offset, &seq_num, sizeof(seq_num));
    offset += sizeof(seq_num);

    std::memcpy(buffer + offset, &timestamp, sizeof(timestamp));
    offset += sizeof(timestamp);

    std::memcpy(buffer + offset, msg.data(), msg.size());
    offset += msg.size();

    ssize_t sent = write(sock_, buffer, offset);
    if (sent < 0) perror("write");

    std::cout << "[TcpTransport] Sent seq=" << seq_num << " to " << config_.iface_ip
              << " port=" << map_topic_to_port(topic) << std::endl;
}

void TcpTransport::server_loop() {
    while (running_) {
        sockaddr_in client_addr{};
        socklen_t addrlen = sizeof(client_addr);

        std::cout << "[TcpTransport] Waiting for connection..." << std::endl;

        int client_sock = accept(sock_, (sockaddr*)&client_addr, &addrlen);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        std::cout << "[TcpTransport] Client connected!" << std::endl;

        std::lock_guard<std::mutex> lock(clients_mutex_);

        // Spawn thread for this client
        client_threads_.emplace_back(&TcpTransport::client_recv_loop, this, client_sock);
        client_socks_.push_back(client_sock);
    }
}

void TcpTransport::add_receive_callback(ReceiveCallback cb) {
    std::lock_guard<std::mutex> lock(callbacks_mutex_);
    callbacks_.push_back(cb);
    std::cout << "[TcpTransport] Registered subscriber callback (" << callbacks_.size() << " total)" << std::endl;
}

void TcpTransport::client_recv_loop(int client_sock) {
    char buffer[1500];

    while (running_) {
        ssize_t len = read(client_sock, buffer, sizeof(buffer)-1);
        if (len < 0) {
            perror("read");
            break;
        }
        else if (len == 0) {
            std::cout << "[TcpTransport] Client disconnected" << std::endl;
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

        std::lock_guard<std::mutex> lock(callbacks_mutex_);
            // 1️⃣ Dispatch to local subs
{
    std::lock_guard<std::mutex> lock(callbacks_mutex_);
        for (auto& cb : callbacks_) {
            cb(topic_, seq, timestamp, payload);
            }
        }

    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (auto csock : client_socks_) {
            if (csock == client_sock) continue;  // Optional: skip sender

            char buffer[1500];
            size_t offset = 0;

            std::memcpy(buffer + offset, &seq, sizeof(seq));
            offset += sizeof(seq);

            std::memcpy(buffer + offset, &timestamp, sizeof(timestamp));
            offset += sizeof(timestamp);

            std::memcpy(buffer + offset, payload.data(), payload.size());
            offset += payload.size();

            ssize_t sent = write(csock, buffer, offset);
            if (sent < 0) perror("write (broadcast)");
            else {
                std::cout << "[TcpTransport] Broadcast to client socket=" << csock
                        << " msg=" << payload << std::endl;
            }
        }
    }
    }

    close(client_sock);
}
