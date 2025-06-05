#include "Node.h"
#include "TypedServiceClient.h"
#include "messages/Service.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    TransportConfig config;
    auto node = std::make_shared<Node>("example_service_client", config);

    auto client = node->create_typed_service_client<SumRequest, SumResponse>("sum_service");

    int count = 0;
    while (true) {
        SumRequest req;
        req.num1 = count;
        req.num2 = count + 10;

        client->call(req);

        if (client->wait_for_response(500)) {
            SumResponse resp = client->get_latest_response();
            std::cout << "[Client] " << req.num1 << " + " << req.num2 << " = " << resp.sum << std::endl;
        } else {
            std::cout << "[Client] Timeout waiting for response." << std::endl;
        }

        count++;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
