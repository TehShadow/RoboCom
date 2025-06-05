#include "UdpTransport.h"
#include "Node.h"
#include "Publisher.h"
#include "TypedSubscriber.h"
#include "Serialization.h"
#include "messages/Pose.h"

#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // UDP Transport config: enable multicast for real test
    TransportConfig config;
    config.iface_ip = "127.0.0.1";  // localhost
    config.use_multicast = false;  // set to true if you want real multicast

    auto udp_pub_transport = std::make_shared<UdpTransport>(config);
    auto udp_sub_transport = std::make_shared<UdpTransport>(config, "pose_topic");  // Bind to topic port

    auto pub = std::make_shared<Publisher>("pose_topic", udp_pub_transport);

    auto node = std::make_shared<Node>("udp_node", udp_sub_transport);
    auto sub = node->create_typed_subscriber<Pose>("pose_topic");

    int count = 0;
    while (true) {
        Pose msg;
        msg.x = count * 1.0f;
        msg.y = count * 2.0f;
        msg.theta = count * 0.1f;

        auto buffer = serialize(msg);
        pub->publish_raw(buffer);

        std::cout << "[UDP Publisher] Pose x=" << msg.x << " y=" << msg.y << " theta=" << msg.theta << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        Pose latest = sub->get_latest();
        std::cout << "[UDP Subscriber] Latest Pose x=" << latest.x << " y=" << latest.y << " theta=" << latest.theta << std::endl;

        count++;
    }

    return 0;
}
