#include "UdpTransport.h"
#include "Node.h"
#include "Serialization.h"
#include "messages/Pose.h"

#include <signal.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    signal(SIGPIPE, SIG_IGN);

    std::string node_name = "udp_publisher";

    TransportConfig config;
    config.use_multicast = true;
    config.iface_ip = "127.0.0.1";  // or your iface

    auto transport = std::make_shared<UdpTransport>(config, "pose_topic");

    Node node(node_name, transport);

    auto pub = node.create_publisher("pose_topic");

    uint32_t count = 0;

    while (true) {
        Pose pose;
        pose.x = count * 1.0f;
        pose.y = count * 2.0f;
        pose.theta = count * 0.1f;

        auto buffer = serialize(pose);

        pub->publish_raw(buffer);

        std::cout << "[udp_publisher] Published Pose: x=" << pose.x
                  << " y=" << pose.y
                  << " theta=" << pose.theta << std::endl;

        count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
