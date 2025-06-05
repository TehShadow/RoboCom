#include "Node.h"
#include "messages/Service.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    auto node = std::make_shared<Node>("example_service_server", config);

    auto server = node->create_service_server<SumRequest, SumResponse>(
        "sum_service",
        [](const SumRequest& req) -> SumResponse {
            SumResponse resp;
            resp.sum = req.num1 + req.num2;
            std::cout << "[Server] Received: " << req.num1 << " + " << req.num2
                      << " = " << resp.sum << std::endl;
            return resp;
        });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
