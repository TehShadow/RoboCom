#include "TcpTransport.h"
#include "Node.h"
#include "Publisher.h"
#include "TypedSubscriber.h"
#include "Serialization.h"
#include "messages/Pose.h"

#include <iostream>
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    config.iface_ip = "127.0.0.1";  // Connect to localhost server

    // TCP Transport: Server
    auto tcp_server_transport = std::make_shared<TcpTransport>(config, "pose_topic", true);
    // TCP Transport: Client
    auto tcp_client_transport = std::make_shared<TcpTransport>(config, "pose_topic", false);

    // Server acts as subscriber
    auto node = std::make_shared<Node>("tcp_server_node", tcp_server_transport);
    auto sub = node->create_typed_subscriber<Pose>("pose_topic");

    // Client acts as publisher
    auto pub = std::make_shared<Publisher>("pose_topic", tcp_client_transport);

    int count = 0;
    while (true) {
        Pose msg;
        msg.x = count * 1.0f;
        msg.y = count * 2.0f;
        msg.theta = count * 0.1f;

        auto buffer = serialize(msg);
        pub->publish_raw(buffer);

        std::cout << "[TCP Publisher] Pose x=" << msg.x << " y=" << msg.y << " theta=" << msg.theta << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        Pose latest = sub->get_latest();
        std::cout << "[TCP Subscriber] Latest Pose x=" << latest.x << " y=" << latest.y << " theta=" << latest.theta << std::endl;

        count++;
    }

    return 0;
}
