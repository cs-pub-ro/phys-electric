#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "tusb.h"
#include <string>
#include <iostream>

#include "ethernet/ethernet.hpp"
#include "manchester_codec/manchester.hpp"

// GPIO defines
#define BAUD_RATE 9600
#define TX_PIN 12
#define RX_PIN 13
#define CLOCK_PERIOD_US 104

// Ethernet defines
const uint8_t source_mac_address[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
const uint8_t destination_mac_address[] = {0xFE, 0xCA, 0xEF, 0xBE, 0xAD, 0xDE};
const uint16_t ether_type = 0x0800;

// Data to send
uint8_t sending_data[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
uint32_t sending_data_len = sizeof(sending_data);

using namespace std;

void sender(Manchester& manchester)
{
    Ethernet ethernet;
    ethernet.init(source_mac_address, destination_mac_address, ether_type);

    // Encapsulate the payload into an Ethernet frame
    vector<uint8_t> frame = ethernet.eth_encap(sending_data, sending_data_len);

    while(1) {
        //DEBUG: printing the frame
        manchester.send_debug_print(frame.data(), frame.size());

        // Send the frame
        manchester.send_manchester(frame.data(), frame.size());

        sleep_ms(250);
    }
}

void receiver(Manchester& manchester)
{
    Ethernet ethernet;
    ethernet.init(destination_mac_address, source_mac_address, ether_type);

    // Expecting the same byte payload as the sender
    size_t frame_len = MAC_HEADER_LEN + sending_data_len + FCS_LEN;
    uint8_t frame[frame_len];

    while(1) {
        memset(frame, 0, sizeof(frame));

        // Receive the frame
        manchester.recv_manchester(frame, frame_len);

        //DEBUG: printing the frame
        manchester.recv_debug_print(frame, frame_len);

        vector<uint8_t> decap_frame = ethernet.eth_decap(frame, frame_len, sending_data_len);
        if(decap_frame.size() == 0) {
            continue;
        }
        
        printf("Received message: ");
        for (size_t i = 0; i < decap_frame.size(); i++) {
            printf("%c", decap_frame[i]);
        }
        printf("\n");
    }
}

int main()
{
    stdio_init_all();

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Power on the Wi-Fi LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // Initialise Manchester
    Manchester manchester;
    manchester.init(TX_PIN, RX_PIN, BAUD_RATE, CLOCK_PERIOD_US);

    string type = "sender";

    if (type == "sender") {
        sender(manchester);
    } else if (type == "receiver") {
        receiver(manchester);
    } else {
        printf("Invalid type. Choose between \"sender\" and \"receiver\" \n");
        return -1;
    }

    return 0;
}
