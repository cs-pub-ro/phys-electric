#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <stdint.h>
#include "pico/stdlib.h"
#include "../manchester_codec/sender.h"
#include "../manchester_codec/receiver.h"
#include <unistd.h>

class Ethernet {
private:
    static constexpr size_t MAC_ADDRESS_LEN = 6;
    static constexpr size_t ETHER_HEADER_LEN = 14;
    uint8_t preamble[8] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB };
    uint8_t source_mac[MAC_ADDRESS_LEN];
    uint8_t destination_mac[MAC_ADDRESS_LEN];
    uint16_t ether_type;
    uint32_t fcs;

    uint32_t calculate_fcs(const uint8_t* data, size_t length);
public:
    void init(const uint8_t* source_mac_address, const uint8_t* destination_mac_address, uint16_t type);

    void send_ethernet(Sender& sender, uint8_t* data, uint32_t length);
    void receive_ethernet(Receiver& receiver, uint8_t* data, uint32_t length);
};

#endif // ETHERNET_HPP