#pragma once
#include <cstdint>
#include <vector>

struct CanMessage {
    double timestamp;       // thời gian (s)
    uint8_t channel;        // CAN channel
    uint32_t can_id;        // ID
    uint8_t dlc;            // data length
    std::vector<uint8_t> data; // payload
};
