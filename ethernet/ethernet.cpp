#include "ethernet.hpp"

void Ethernet::init(const uint8_t *source_mac_address, const uint8_t *destination_mac_address, uint16_t type)
{
    memcpy(source_mac, source_mac_address, MAC_ADDRESS_LEN);
    memcpy(destination_mac, destination_mac_address, MAC_ADDRESS_LEN);
    ether_type = type;
}

uint32_t Ethernet::calculate_fcs(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++)
    {
        uint8_t byte = data[i];
        for (int bit = 0; bit < 8; bit++)
        {
            bool bit_crc = (crc & 1) != 0;
            bool bit_data = (byte & (1 << bit)) != 0;
            crc = (crc >> 1) ^ ((bit_crc ^ bit_data) ? 0xEDB88320 : 0);
        }
    }
    return ~crc;
}

vector<uint8_t> Ethernet::eth_encap(uint8_t *payload, uint32_t payload_len)
{
    const size_t total_frame_size = MAC_HEADER_LEN + payload_len + FCS_LEN;
    uint8_t frame[total_frame_size] = {0};

    // Current position tracker for frame construction
    size_t current_pos = 0;

    // Destination MAC
    memcpy(frame + current_pos, destination_mac, MAC_ADDRESS_LEN);
    current_pos += MAC_ADDRESS_LEN;

    // Source MAC
    memcpy(frame + current_pos, source_mac, MAC_ADDRESS_LEN);
    current_pos += MAC_ADDRESS_LEN;

    // EtherType
    memcpy(frame + current_pos, &ether_type, sizeof(ether_type));
    current_pos += sizeof(ether_type);

    // Payload
    memcpy(frame + current_pos, payload, payload_len);
    current_pos += payload_len;

    // FCS
    fcs = calculate_fcs(frame, MAC_HEADER_LEN + payload_len);
    memcpy(frame + current_pos, &fcs, sizeof(fcs));

    return vector<uint8_t>(frame, frame + sizeof(frame));
}

vector<uint8_t> Ethernet::eth_decap(uint8_t *frame, uint32_t frame_data_length, uint32_t exp_len)
{
    // Extract the received FCS
    uint32_t received_fcs;
    memcpy(&received_fcs, frame + MAC_HEADER_LEN + exp_len, sizeof(received_fcs));

    // Calculate the FCS
    uint32_t calculated_fcs = calculate_fcs(frame, MAC_HEADER_LEN + exp_len);

    // Compare FCS
    if (received_fcs != calculated_fcs) {
        printf("Frame Check Sequence (FCS) error!\n");
        return vector<uint8_t>();
    }

    // Compare EtherType
    uint16_t received_ether_type;
    memcpy(&received_ether_type, frame + 2 * MAC_ADDRESS_LEN, sizeof(received_ether_type));
    if (received_ether_type != ether_type) {
        printf("EtherType error!\n");
        return vector<uint8_t>();
    }

    // Compare MACs
    if (memcmp(frame, source_mac, MAC_ADDRESS_LEN) != 0) {
        printf("Destination MAC error!\n");
        return vector<uint8_t>();
    }    

    return vector<uint8_t>(frame + MAC_HEADER_LEN, frame + MAC_HEADER_LEN + exp_len);
}
