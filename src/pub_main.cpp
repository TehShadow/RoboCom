#include "PeerToPeerTcpTransport.h"
#include "Node.h"
#include "Serialization.h"
#include "messages/Pose.h"
#include <signal.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    signal(SIGPIPE, SIG_IGN);

    std::string node_name = "tcp_publisher";

    TransportConfig config;

    auto transport = std::make_shared<PeerToPeerTcpTransport>(config, "pose_topic");

    Node node(node_name, transport);

    auto pub = node.create_publisher("pose_topic");

    // 🚀 Just start discovery!
    transport->get_discovery()->start();

    uint32_t count = 0;

    while (true) {
        Pose pose;
        pose.x = count * 1.0f;
        pose.y = count * 2.0f;
        pose.theta = count * 0.1f;

        auto buffer = serialize(pose);

        pub->publish_raw(buffer);

        std::cout << "[tcp_publisher] Published Pose: x=" << pose.x
                  << " y=" << pose.y
                  << " theta=" << pose.theta << std::endl;

        count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
