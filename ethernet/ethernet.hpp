#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdio.h>
#include "pico/stdlib.h"
#include <vector>

using namespace std;

#define MAC_ADDRESS_LEN 6
#define MAC_HEADER_LEN (2 * MAC_ADDRESS_LEN + 2) // 6 + 6 + 2
#define FCS_LEN 4

class Ethernet {
private:
    uint8_t source_mac[MAC_ADDRESS_LEN];
    uint8_t destination_mac[MAC_ADDRESS_LEN];
    uint16_t ether_type;
    uint32_t fcs;

    uint32_t calculate_fcs(const uint8_t* data, size_t length);
public:
    void init(const uint8_t* source_mac_address, const uint8_t* destination_mac_address, uint16_t type);

    vector<uint8_t> eth_encap(uint8_t *payload, uint32_t payload_len);
    vector<uint8_t> eth_decap(uint8_t *frame, uint32_t frame_data_length, uint32_t exp_len);
};

#endif // ETHERNET_HPP