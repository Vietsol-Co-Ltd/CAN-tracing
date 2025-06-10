//SharedMemory.hh

#pragma once
#include <vector>
#include <string>

struct CanMessage {
    std::string signalName;
    std::string timeStamp;
    std::string deltaTime;
    std::string messageName;
    std::string channel;
    std::string id;
    std::string direction;
    std::string dlc;
    std::string dataBytes;
    std::string busType;
    std::string cycleTime;
    std::string flags;
};

namespace lblf {
namespace print {
    extern std::vector<CanMessage> allCanMessages; // <- extern để dùng toàn cục
}
}