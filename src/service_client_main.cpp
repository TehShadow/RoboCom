#include "PeerToPeerTcpTransport.h"
#include "Node.h"
#include "TypedServiceClient.h"
#include "messages/Service.h"
#include <signal.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    signal(SIGPIPE, SIG_IGN);

    std::string node_name = "sum_service_client";

    TransportConfig config;

    // 🚀 Create Node with TransportManager inside
    auto node = std::make_shared<Node>(node_name, config);

    // 🚀 TypedServiceClient will now use Node's TransportManager
    auto client = std::make_shared<TypedServiceClient<SumRequest, SumResponse>>(node, "sum_service");

    int count = 0;

    while (true) {
        SumRequest req;
        req.num1 = count;
        req.num2 = count + 10;

        client->call(req);

        if (client->wait_for_response(500)) {
            SumResponse latest_resp = client->get_latest_response();
            std::cout << "[sum_service_client] Request: " << req.num1 << " + " << req.num2
                      << " → Response: " << latest_resp.sum << std::endl;
        } else {
            std::cout << "[sum_service_client] Timeout waiting for response!" << std::endl;
        }

        count++;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
