#include "Publisher.h"
#include <thread>
#include <chrono>

int main() {
    Publisher pub("robot/pose");

    int count = 0;
    while (true) {
        std::string msg = "Pose update " + std::to_string(count++);
        pub.publish(msg);
        std::cout << "Published: " << msg << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10Hz
    }

    return 0;
}
