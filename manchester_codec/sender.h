#ifndef SENDER_H
#define SENDER_H

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdint.h>

class Sender {
private:
    uint8_t tx_pin;
    uint32_t baud_rate;
    uint32_t clock_period_us;
    uart_inst_t *uart_instance = uart0;

public:
    void init(uint8_t tx_pin, uint32_t baud_rate, uint32_t clock_period_us, uart_inst_t *uart_instance);
    void send_manchester_bit(int bit);
    void send_manchester_byte(int byte);
};

#endif // SENDER_H