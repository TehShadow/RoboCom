#include "Node.h"
#include "TcpTransport.h"
#include "Transport.h"
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    config.iface_ip = "192.168.1.A";  // 🚨 Replace with SERVER IP (Robot A)

    // CLIENT MODE:
    auto transport = std::make_shared<TcpTransport>(config, "robot/pose", false);

    Node node("robot_node_sub_tcp_client", transport);

    auto sub = node.create_subscriber("robot/pose", [](uint32_t seq, uint64_t latency_us, const std::string& msg) {
        std::cout << "[TCP_CLIENT_SUB] SEQ=" << seq
                  << " LAT=" << latency_us
                  << " MSG=" << msg << std::endl;
    });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
