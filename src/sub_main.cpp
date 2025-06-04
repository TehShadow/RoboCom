#include "PeerToPeerTcpTransport.h"
#include "Discovery.h"
#include "Node.h"
#include "Serialization.h"
#include "messages/Pose.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    std::string node_name = "subscriber";

    TransportConfig config;
    auto transport = std::make_shared<PeerToPeerTcpTransport>(config, "pose_topic");

    auto discovery = std::make_shared<Discovery>("pose_topic");

    Node node(node_name, transport, discovery);

    auto sub = node.create_subscriber("pose_topic",
        [node_name](uint32_t seq, uint64_t latency_us, const std::vector<uint8_t>& buffer) {
            Pose pose = deserialize<Pose>(buffer);

            std::cout << "[" << node_name << "] Received Pose: x=" << pose.x
                    << " y=" << pose.y
                    << " theta=" << pose.theta
                    << " (seq=" << seq << ", latency=" << latency_us << " us)" << std::endl;
        });

    // Spin forever
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
