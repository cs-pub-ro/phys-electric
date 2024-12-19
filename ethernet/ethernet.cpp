#include "ethernet.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdio.h>

// Assume these are defined somewhere or define them here:
#define MAC_ADDRESS_LEN 6
// ETHER_HEADER_LEN = preamble(8) + dest MAC(6) + source MAC(6) + EtherType(2) = 8+6+6+2 = 22
#define ETHER_HEADER_LEN (sizeof(preamble) + (MAC_ADDRESS_LEN * 2) + sizeof(ether_type))

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

#define PREAMBLE_LEN 8 // 7 bytes of 0xAA + 1 byte of 0xAB
#define MAC_ADDRESS_LEN 6
#define MAC_HEADER_LEN (2 * MAC_ADDRESS_LEN + 2) // 6 + 6 + 2
#define FCS_LEN 4
#define ETHER_HEADER_LEN (PREAMBLE_LEN + MAC_HEADER_LEN) // 8 + 14 = 22

void Ethernet::send_ethernet(Sender &sender, uint8_t *data, uint32_t length)
{
    uint8_t frame[ETHER_HEADER_LEN + length + sizeof(fcs)];

    // Step 1: Copy Preamble and SFD
    memcpy(frame, preamble, sizeof(preamble));

    // Step 2: Add Destination MAC
    memcpy(frame + PREAMBLE_LEN, destination_mac, MAC_ADDRESS_LEN);

    // Step 3: Add Source MAC
    memcpy(frame + PREAMBLE_LEN + MAC_ADDRESS_LEN, source_mac, MAC_ADDRESS_LEN);

    // Step 4: Add EtherType
    memcpy(frame + PREAMBLE_LEN + 2 * MAC_ADDRESS_LEN, &ether_type, sizeof(ether_type));

    // Step 5: Add Payload
    memcpy(frame + PREAMBLE_LEN + MAC_HEADER_LEN, data, length);

    // Step 6: Calculate FCS over [Dest MAC | Src MAC | EtherType | Payload]
    fcs = calculate_fcs(frame + PREAMBLE_LEN, MAC_HEADER_LEN + length);
    memcpy(frame + PREAMBLE_LEN + MAC_HEADER_LEN + length, &fcs, sizeof(fcs));

    // Debug: Print Sending Frame
    printf("Sending frame (hex): ");
    for (size_t i = 0; i < sizeof(frame); ++i) {
        printf("%02X ", frame[i]);
    }
    printf("|| CRC: %08X\n", fcs);

    // Step 7: Send Frame using Manchester Encoding
    for (size_t i = 0; i < sizeof(frame); i++) {
        sender.send_manchester_byte(frame[i]);
    }
}

void Ethernet::receive_ethernet(Receiver &receiver, uint8_t *data, uint32_t length)
{
    // Step 1: Detect Preamble and SFD
    size_t matched = 0;
    while (matched < PREAMBLE_LEN) {
        uint8_t byte = receiver.receive_manchester_byte();
        if (byte == preamble[matched]) {
            matched++;
        } else {
            matched = (byte == preamble[0]) ? 1 : 0;
        }
    }

    // Step 2: Allocate memory for MAC headers, Payload, and FCS
    size_t frame_data_length = MAC_HEADER_LEN + length + FCS_LEN;
    uint8_t frame[frame_data_length];

    // Step 3: Read the frame data
    for (size_t i = 0; i < frame_data_length; i++) {
        frame[i] = receiver.receive_manchester_byte();
    }

    // Step 4: Extract the received FCS
    uint32_t received_fcs;
    memcpy(&received_fcs, frame + MAC_HEADER_LEN + length, sizeof(received_fcs));

    // Step 5: Calculate the FCS over [Dest MAC | Src MAC | EtherType | Payload]
    uint32_t calculated_fcs = calculate_fcs(frame, MAC_HEADER_LEN + length);
    printf("Calculated CRC: %08X\n", calculated_fcs);

    // Step 6: Verify FCS
    if (received_fcs != calculated_fcs) {
        printf("Frame Check Sequence (FCS) error!\n");
        return; // Discard this frame
    }

    // Step 7: Extract Payload
    memcpy(data, frame + MAC_HEADER_LEN, length);

    // Debug: Print Received Frame
    printf("Received frame (hex): ");
    for (size_t i = 0; i < frame_data_length; ++i) {
        printf("%02X ", frame[i]);
    }
    printf("\n");
}
