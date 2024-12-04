#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "tusb.h"
#include "manchester_codec/sender.h"
#include "manchester_codec/receiver.h"
#include <string>
#include <iostream>

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define BAUD_RATE 9600

#define UART_TX_PIN 12
#define UART_RX_PIN 13

#define CLOCK_PERIOD_US 100

using namespace std;

int main()
{
    string type;
    cin >> type;

    stdio_init_all();

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

 
    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // // Read a     character from UART and echo it back
    // if (uart_is_readable(UART_ID)) {
    //     char ch = uart_getc(UART_ID);
    //     //uart_putc(UART_ID, ch);
    //     printf("%c", ch);
    // }

    if (type == "sender") {
        Sender sender;
        sender.init(UART_TX_PIN, BAUD_RATE, CLOCK_PERIOD_US, UART_ID);
        
        while (true) {
            sender.send_manchester_byte('A');
            sleep_ms(1000);
        }
        
    } else if (type == "receiver") {
        Receiver receiver;
        receiver.init(UART_RX_PIN, BAUD_RATE, CLOCK_PERIOD_US, UART_ID);
        
        while (true) {
    
            int received_byte = receiver.receive_manchester_byte();
            if (received_byte != -1) {
                printf("Received byte: 0x%02X\n", received_byte);
            } else {
                printf("Error: Invalid Manchester encoding\n");
            }
            sleep_ms(1000);
        }

    } else {
        printf("Invalid type. Choose between \"sender\" and \"receiver\" \n");
        return -1;
    }
}
