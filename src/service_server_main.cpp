#include "PeerToPeerTcpTransport.h"
#include "Node.h"
#include "Serialization.h"
#include "messages/Service.h"
#include <signal.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    signal(SIGPIPE, SIG_IGN);

    std::string node_name = "sum_service_server";

    TransportConfig config;

    // 🚀 Create Node with TransportManager inside
    auto node = std::make_shared<Node>(node_name, config);

    // 🚀 TypedServiceServer will now use Node's TransportManager
    auto server = node->create_typed_service_server<SumRequest, SumResponse>(
        "sum_service",
        [](const SumRequest& req) -> SumResponse {
            SumResponse resp;
            resp.sum = req.num1 + req.num2;
            std::cout << "[sum_service_server] Received: " << req.num1 << " + " << req.num2
                      << " = " << resp.sum << std::endl;
            return resp;
        });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
