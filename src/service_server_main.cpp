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

    std::string node_name = "service_server";

    TransportConfig config;

    auto transport = std::make_shared<PeerToPeerTcpTransport>(config, "pose_service");

    auto node = std::make_shared<Node>(node_name, transport);

    auto server = node->create_typed_service_server<Pose, Pose>(
        "pose_service",
        [](const Pose& req) -> Pose {
            Pose resp;
            resp.x = req.x * 10.0f;
            resp.y = req.y * 10.0f;
            resp.theta = req.theta * 10.0f;
            std::cout << "[service_server] Handled request: x=" << req.x
                      << " y=" << req.y
                      << " theta=" << req.theta << std::endl;
            return resp;
        });

    transport->get_discovery()->start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
