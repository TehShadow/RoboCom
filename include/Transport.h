#pragma once
#include <string>
#include <functional>
#include <cstdint>

class Transport {
public:
    virtual ~Transport() {}

    virtual void send(const std::string& topic, const std::string& msg, uint32_t seq_num, uint64_t timestamp) = 0;

    using ReceiveCallback = std::function<void(const std::string& topic, uint32_t seq, uint64_t timestamp, const std::string& msg)>;
    virtual void set_receive_callback(ReceiveCallback cb) = 0;
};

struct TransportConfig {
    std::string iface_ip = "127.0.0.1";
    std::string multicast_addr = "239.255.0.1";
    bool use_multicast = false;
};
