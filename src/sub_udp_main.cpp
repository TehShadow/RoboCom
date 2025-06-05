#include "UdpTransport.h"
#include "Node.h"
#include "TypedSubscriber.h"
#include "Serialization.h"
#include "messages/Pose.h"
#include <signal.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    signal(SIGPIPE, SIG_IGN);

    std::string node_name = "udp_subscriber";

    TransportConfig config;
    config.use_multicast = true;
    config.iface_ip = "127.0.0.1";

    auto transport = std::make_shared<UdpTransport>(config, "pose_topic");

    auto node = std::make_shared<Node>(node_name, transport);

    auto sub = node->create_typed_subscriber<Pose>("pose_topic");

    while (true) {
        Pose latest_pose = sub->get_latest();
        std::cout << "[" << node_name << "] Latest Pose: x=" << latest_pose.x
                  << " y=" << latest_pose.y
                  << " theta=" << latest_pose.theta << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
