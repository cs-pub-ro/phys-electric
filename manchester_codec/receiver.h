#ifndef RECEIVER_H
#define RECEIVER_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

class Receiver {
private:
    uint8_t rx_pin;
    uint32_t baud_rate;
    uint32_t clock_period_us;
    uart_inst_t *uart_instance = uart0;

public:
    void init(uint8_t rx_pin, uint32_t baud_rate, uint32_t clock_period_us, uart_inst_t *uart_instance);
    int receive_manchester_bit(int first_half, int second_half);
    int receive_manchester_byte();
};

#endif // RECEIVER_H