#include "Node.h"
#include "TcpTransport.h"
#include "Transport.h"
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    config.iface_ip = "127.0.0.1";

    auto transport = std::make_shared<TcpTransport>(config, "robot/pose", true);

    Node node("robot_node_sub_tcp", transport);

    auto sub = node.create_subscriber("robot/pose", [](uint32_t seq, uint64_t latency_us, const std::string& msg) {
        std::cout << "Received: SEQ=" << seq
                  << " LAT=" << latency_us
                  << " MSG=" << msg << std::endl;
    });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
