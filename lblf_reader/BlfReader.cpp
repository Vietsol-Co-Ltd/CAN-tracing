#include "BlfReader.hpp"
#include "lblf/blf_reader.hh"
#include "lblf/blf_structs.hh"

std::vector<CanMessage> loadCanMessagesFromBlf(const std::string& filepath) {
    std::vector<CanMessage> result;

    lblf::blf_reader reader(filepath);
    while (reader.next()) {
        auto data = reader.data();
        if (data.base_header.objectType == lblf::ObjectType_e::CAN_MESSAGE2) {
            // Create a struct to hold CAN message data
            struct CanMsg {
                uint32_t timestamp;
                uint8_t channel;
                uint32_t id;
                uint8_t dlc;
                uint8_t data[8];
            } can2;

            // Read the CAN message data
            lblf::read_blf_struct(data, can2);

            // Convert to our CanMessage format
            CanMessage msg;
            msg.timestamp = can2.timestamp / 1000000.0; // Convert to seconds
            msg.channel = can2.channel;
            msg.can_id = can2.id;
            msg.dlc = can2.dlc;
            msg.data.assign(can2.data, can2.data + can2.dlc);

            result.push_back(std::move(msg));
        }
    }

    return result;
}
