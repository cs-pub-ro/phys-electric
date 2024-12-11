#include "sender.h"

void Sender::init(uint8_t tx_pin, uint32_t baud_rate, uint32_t clock_period_us)
{
    this->tx_pin = tx_pin;
    this->baud_rate = baud_rate;
    this->clock_period_us = clock_period_us;
}

void Sender::send_manchester_bit(int bit)
{
    if (bit == 1) {
        gpio_put(this->tx_pin, 0);
        busy_wait_us(this->clock_period_us / 2);
        gpio_put(this->tx_pin, 1);
    } else {
        gpio_put(this->tx_pin, 1);
        busy_wait_us(this->clock_period_us / 2);
        gpio_put(this->tx_pin, 0);
    }
    busy_wait_us(this->clock_period_us / 2);

}

void Sender::send_manchester_byte(int byte)
{
    for (int i = 7; i >= 0; i--) {
        int bit = (byte >> i) & 1;
        send_manchester_bit(bit);
    }
}