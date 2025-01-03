#ifndef MANCHESTER_HPP
#define MANCHESTER_HPP

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdint.h>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

#define PREAMBLE_LEN 8 // 7 bytes of 0xAA + 1 byte of 0xAB
class Manchester {
private:
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint32_t baud_rate;
    uint32_t clock_period_us;
    const uint8_t preamble[8] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB };

    // Manchester encoding/decoding functions
    int receive_manchester_bit(int first_half, int second_half);
    int receive_manchester_byte();
    void send_manchester_bit(int bit);
    void send_manchester_byte(int byte);

    // Sync functions
    vector<uint8_t> add_preamble(uint8_t *frame, size_t length);
    void wait_for_preamble();
public:
    void init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baud_rate, uint32_t clock_period_us);

    // Send functions
    void send_debug_print(uint8_t *frame, size_t length);
    void send_manchester(uint8_t *data, uint32_t length);

    // Receive functions
    void recv_manchester(uint8_t *data, uint32_t length);
    void recv_debug_print(uint8_t *data, uint32_t length);
};

#endif // MANCHESTER_HPP