#pragma once
#include <cstdint>

struct MyService_Request {
    int32_t a;
    int32_t b;
};

struct MyService_Response {
    int32_t sum;
};

struct SumRequest {
    int32_t num1;
    int32_t num2;
};

struct SumResponse {
    int32_t sum;
};