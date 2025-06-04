#include "Node.h"
#include "UdpTransport.h"
#include "Transport.h"
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    config.iface_ip = "127.0.0.1";
    config.use_multicast = false;

    auto transport = std::make_shared<UdpTransport>(config);
    Node node("robot_node_pub", transport);

    auto pub = node.create_publisher("robot/pose");

    int count = 0;
    while (true) {
        pub->publish("Pose update " + std::to_string(count++));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
