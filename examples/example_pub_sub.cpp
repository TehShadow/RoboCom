#include "Node.h"
#include "TypedSubscriber.h"
#include "Publisher.h"
#include "Serialization.h"
#include "messages/Pose.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    auto node = std::make_shared<Node>("example_pub_sub", config);

    auto pub = node->create_publisher("pose_topic");

    auto sub = node->create_typed_subscriber<Pose>("pose_topic");

    int count = 0;
    while (true) {
        Pose msg;
        msg.x = count * 1.0f;
        msg.y = count * 2.0f;
        msg.theta = count * 0.1f;

        auto buffer = serialize(msg);
        pub->publish_raw(buffer);

        std::cout << "[Publisher] Pose x=" << msg.x << " y=" << msg.y << " theta=" << msg.theta << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        Pose latest = sub->get_latest();
        std::cout << "[Subscriber] Latest Pose x=" << latest.x << " y=" << latest.y << " theta=" << latest.theta << std::endl;

        count++;
    }

    return 0;
}
