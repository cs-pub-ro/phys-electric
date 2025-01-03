#include "manchester.hpp"

void Manchester::init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baud_rate, uint32_t clock_period_us)
{
    this->tx_pin = tx_pin;
    this->rx_pin = rx_pin;
    this->baud_rate = baud_rate;
    this->clock_period_us = clock_period_us;

    // Set up the TX pin
    gpio_init(this->tx_pin);
    gpio_set_dir(this->tx_pin, GPIO_OUT);

    // Set up the RX pin
    gpio_init(this->rx_pin);
    gpio_set_dir(this->rx_pin, GPIO_IN);
}

int Manchester::receive_manchester_bit(int first_half, int second_half) {
    if (first_half == 0 && second_half == 1) {
            return 1;
        } else if (first_half == 1 && second_half == 0) {
            return 0;
        }
        return -1;
}

int Manchester::receive_manchester_byte()
{
    int byte = 0;
    for (int i = 7; i >= 0; i--) {
        int first_half = gpio_get(this->rx_pin);
        busy_wait_us(this->clock_period_us / 2);

        int second_half = gpio_get(this->rx_pin);
        busy_wait_us(this->clock_period_us / 2);

        int bit = Manchester::receive_manchester_bit(first_half, second_half);
        if (bit == -1) {
            return -1;
        }

        byte = (byte << 1) | bit;
    }
    return byte;
}

void Manchester::send_manchester_bit(int bit)
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

void Manchester::send_manchester_byte(int byte)
{
    for (int i = 7; i >= 0; i--) {
        int bit = (byte >> i) & 1;
        send_manchester_bit(bit);
    }
}

void Manchester::send_debug_print(uint8_t *frame, size_t length)
{
    printf("Sending frame (hex): ");
    for (size_t i = 0; i < length; i++) {
        printf("%02X ", frame[i]);
    }
    printf("\n");
}

void Manchester::send_manchester(uint8_t *data, uint32_t length)
{
    vector<uint8_t> frame_with_preamble = add_preamble(data, length);

    for (size_t i = 0; i < frame_with_preamble.size(); i++) {
        send_manchester_byte(frame_with_preamble[i]);
    }
}

vector<uint8_t> Manchester::add_preamble(uint8_t *frame, size_t length)
{
    vector<uint8_t> frame_with_preamble;
    frame_with_preamble.insert(frame_with_preamble.end(), preamble, preamble + PREAMBLE_LEN);
    frame_with_preamble.insert(frame_with_preamble.end(), frame, frame + length);

    return frame_with_preamble;
}

void Manchester::wait_for_preamble()
{
    size_t matched = 0;
    while (matched < PREAMBLE_LEN) {
        uint8_t byte = receive_manchester_byte();
        if (byte == preamble[matched]) {
            matched++;
        } else {
            matched = (byte == preamble[0]) ? 1 : 0;
        }
    }
}

void Manchester::recv_manchester(uint8_t *data, uint32_t length)
{
    wait_for_preamble();

    for (size_t i = 0; i < length; i++) {
        int byte = receive_manchester_byte();
        if (byte == -1) {
            return;
        }
        data[i] = byte;
    }
}

void Manchester::recv_debug_print(uint8_t *data, uint32_t length)
{
    printf("Received frame (hex): ");
    for (size_t i = 0; i < length; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}