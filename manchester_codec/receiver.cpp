#include "receiver.h"

void Receiver::init(uint8_t rx_pin, uint32_t baud_rate, uint32_t clock_period_us, uart_inst_t *uart_instance)
{
    this->rx_pin = rx_pin;
    this->baud_rate = baud_rate;
    this->clock_period_us = clock_period_us;
    this->uart_instance = uart_instance;
}

int Receiver::receive_manchester_bit(int first_half, int second_half) {
    if (first_half == 0 && second_half == 1) {
            return 1;
        } else if (first_half == 1 && second_half == 0) {
            return 0;
        }
        return -1;
}

int Receiver::receive_manchester_byte()
{
    int byte = 0;
    for (int i = 0; i < 8; i++) {
        if (uart_is_readable(this->uart_instance)) {

            int first_half = uart_getc(this->uart_instance);
            busy_wait_us(this->clock_period_us / 2);

            int second_half = uart_getc(this->uart_instance);
            busy_wait_us(this->clock_period_us / 2);

            int bit = Receiver::receive_manchester_bit(first_half, second_half);
            if (bit == -1) {
                return -1;
            }
            byte = (byte << 1) | bit;
        }
    }
    return byte;
}
