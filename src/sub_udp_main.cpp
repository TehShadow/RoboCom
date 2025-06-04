#include "UdpTransport.h"
#include "Discovery.h"
#include "Node.h"
#include "Serialization.h"
#include "messages/Pose.h"
#include <thread>
#include <chrono>
#include <signal.h>
#include <iostream>

int main() {
    signal(SIGPIPE, SIG_IGN);  // 🚀 Not needed for UDP, but safe

    std::string node_name = "udp_subscriber";

    TransportConfig config;
    config.use_multicast = true;  // 🚀 Multicast ON
    config.iface_ip = "0.0.0.0";  // 🚀 Allow binding on all

    // 🚀 Create UDP Transport
    auto transport = std::make_shared<UdpTransport>(config, "pose_topic");

    auto discovery = std::make_shared<Discovery>("pose_topic");  // Optional

    Node node(node_name, transport, discovery);

    auto sub = node.create_subscriber("pose_topic",
        [node_name](uint32_t seq, uint64_t latency_us, const std::vector<uint8_t>& buffer) {
            Pose pose = deserialize<Pose>(buffer);

            std::cout << "[" << node_name << "] Received Pose: x=" << pose.x
                      << " y=" << pose.y
                      << " theta=" << pose.theta
                      << " (seq=" << seq << ", latency=" << latency_us << " us)" << std::endl;
        });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
