#include "receiver.h"

void Receiver::init(uint8_t rx_pin, uint32_t baud_rate, uint32_t clock_period_us)
{
    this->rx_pin = rx_pin;
    this->baud_rate = baud_rate;
    this->clock_period_us = clock_period_us;
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
        // Read the first half of the Manchester bit
        int first_half = gpio_get(this->rx_pin);
        busy_wait_us(this->clock_period_us / 2); // Wait for half the clock period

        // Read the second half of the Manchester bit
        int second_half = gpio_get(this->rx_pin);
        busy_wait_us(this->clock_period_us / 2); // Wait for the remaining half of the clock period

        // Decode the Manchester bit
        int bit = Receiver::receive_manchester_bit(first_half, second_half);
        if (bit == -1) {
            return -1; // Return error for invalid Manchester encoding
        }

        // Shift the bit into the byte
        byte = (byte << 1) | bit;
    }
    return byte;
}
