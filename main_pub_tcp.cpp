#include "Node.h"
#include "TcpTransport.h"
#include "Transport.h"
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    config.iface_ip = "127.0.0.1";

    auto transport = std::make_shared<TcpTransport>(config, "robot/pose", false);

    Node node("robot_node_pub_tcp", transport);

    auto pub = node.create_publisher("robot/pose");

    int count = 0;
    while (true) {
        printf("here %d\n", count);
        pub->publish("Pose update " + std::to_string(count++));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
