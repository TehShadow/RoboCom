#pragma once
#include <string>
#include <functional>
#include <cstdint>



inline uint16_t map_topic_to_port(const std::string& topic) {
    // Simple hash to port: 9000 + hash % 1000 → port range 9000-9999
    size_t hash = std::hash<std::string>{}(topic);
    return 9000 + (hash % 1000);
}