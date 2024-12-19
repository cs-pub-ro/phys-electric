#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "tusb.h"
#include "framing/framing.h"
#include "ethernet/ethernet.hpp"
#include <string>
#include <iostream>

// UART defines
#define BAUD_RATE 9600
#define TX_PIN 12
#define RX_PIN 13
#define CLOCK_PERIOD_US 104

const uint8_t source_mac_address[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
const uint8_t destination_mac_address[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
const uint16_t ether_type = 0x0800;

using namespace std;

int main()
{
    // Change this to "sender" or "receiver" before flashing
    string type = "receiver";

    stdio_init_all();

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Set up the TX pin
    gpio_init(TX_PIN);
    gpio_set_dir(TX_PIN, GPIO_OUT);

    // Set up the RX pin
    gpio_init(RX_PIN);
    gpio_set_dir(RX_PIN, GPIO_IN);

    // Power on the Wi-Fi LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    if (type == "sender") {
        Sender sender;
        sender.init(TX_PIN, BAUD_RATE, CLOCK_PERIOD_US);

        // Example payload
        uint8_t data[] = {'A', 'A', 'C', 'C'};
        Ethernet ethernet;
        ethernet.init(source_mac_address, destination_mac_address, ether_type);

        while(1) {
            printf("Sent frame: \n");
            ethernet.send_ethernet(sender, data, sizeof(data));
            sleep_ms(1000);
        }

    } else if (type == "receiver") {
        Receiver receiver;
        receiver.init(RX_PIN, BAUD_RATE, CLOCK_PERIOD_US);

        Ethernet ethernet;
        ethernet.init(source_mac_address, destination_mac_address, ether_type);

        // Expecting the same byte payload as the sender
        uint8_t buffer[4] = {0};

        while(1) {
            printf("Waiting to receive frame...\n");
            
            // clear buffer
            memset(buffer, 0, sizeof(buffer));

            ethernet.receive_ethernet(receiver, buffer, sizeof(buffer));

            printf("Received frame: ");
            for (size_t i = 0; i < sizeof(buffer); ++i) {
                printf("%2X ", buffer[i]);
            }
            printf("\n");
        }

    } else {
        printf("Invalid type. Choose between \"sender\" and \"receiver\" \n");
        return -1;
    }
}
