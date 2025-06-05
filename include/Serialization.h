#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

template<typename T>
std::vector<uint8_t> serialize(const T& msg) {
    std::vector<uint8_t> buffer(sizeof(T));
    std::memcpy(buffer.data(), &msg, sizeof(T));
    return buffer;
}

template<typename T>
T deserialize(const std::vector<uint8_t>& buffer) {
    T msg;
    std::memcpy(&msg, buffer.data(), sizeof(T));
    return msg;
}

template<typename T>
T deserialize(const std::vector<uint8_t>& buffer, size_t& offset) {
    T msg;
    std::memcpy(&msg, buffer.data() + offset, sizeof(T));
    offset += sizeof(T);
    return msg;
}
