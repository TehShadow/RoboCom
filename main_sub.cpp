#include "Subscriber.h"
#include <thread>
#include <chrono>

int main() {
    Subscriber sub("robot/pose", [](const std::string& msg) {
        std::cout << "Received: " << msg << std::endl;
    });

    // Keep main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
