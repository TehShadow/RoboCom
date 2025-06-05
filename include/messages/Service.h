#pragma once
#include <cstdint>

struct MyService_Request {
    int32_t a;
    int32_t b;
};

struct MyService_Response {
    int32_t sum;
};