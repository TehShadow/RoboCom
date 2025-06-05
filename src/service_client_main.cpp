#include "PeerToPeerTcpTransport.h"
#include "Node.h"
#include "TypedServiceClient.h"
#include "Serialization.h"
#include "messages/Pose.h"

#include <signal.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    signal(SIGPIPE, SIG_IGN);

    std::string node_name = "service_client";

    TransportConfig config;

    auto transport = std::make_shared<PeerToPeerTcpTransport>(config, "pose_service");

    auto node = std::make_shared<Node>(node_name, transport);

    auto client = std::make_shared<TypedServiceClient<Pose, Pose>>(node, "pose_service");

    transport->get_discovery()->start();

    uint32_t count = 0;

    while (true) {
        Pose pose;
        pose.x = count * 1.0f;
        pose.y = count * 2.0f;
        pose.theta = count * 0.1f;

        client->call(pose);

        Pose latest_resp = client->get_latest_response();
        std::cout << "[service_client] Latest Response: x=" << latest_resp.x
                  << " y=" << latest_resp.y
                  << " theta=" << latest_resp.theta << std::endl;

        count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    return 0;
}
