#include "PeerToPeerTcpTransport.h"
#include "Discovery.h"
#include "Node.h"
#include "Serialization.h"
#include "messages/Pose.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    std::string node_name = "publisher";

    TransportConfig config;
    auto transport = std::make_shared<PeerToPeerTcpTransport>(config, "pose_topic");

    auto discovery = std::make_shared<Discovery>("pose_topic");

    Node node(node_name, transport, discovery);

    auto pub = node.create_publisher("pose_topic");

    uint32_t count = 0;

    while (true) {
        Pose pose;
        pose.x = count * 1.0f;
        pose.y = count * 2.0f;
        pose.theta = count * 0.1f;

        // 🚀 Serialize Pose
        auto buffer = serialize(pose);

        // 🚀 Publish raw buffer
        pub->publish_raw(buffer);

        std::cout << "[publisher] Published Pose: x=" << pose.x
                  << " y=" << pose.y << " theta=" << pose.theta << std::endl;

        count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
