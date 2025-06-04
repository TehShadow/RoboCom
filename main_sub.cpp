#include "Node.h"
#include "UdpTransport.h"
#include "Transport.h"
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    config.iface_ip = "127.0.0.1";
    config.use_multicast = false;

    // IMPORTANT: pass topic for binding!
    auto transport = std::make_shared<UdpTransport>(config, "robot/pose");

    Node node("robot_node_sub", transport);

    auto sub = node.create_subscriber("robot/pose", [](uint32_t seq, uint64_t latency_us, const std::string& msg) {
        std::cout << "Received: SEQ=" << seq
                  << " LAT=" << latency_us
                  << " MSG=" << msg << std::endl;
    });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
