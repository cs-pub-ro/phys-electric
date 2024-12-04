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

void on_uart_rx() {
    // Read all available data from UART
    while (uart_is_readable(UART_ID)) {
        // Read the data from UART
        char ch = uart_getc(UART_ID);
        // Send the data to the serial connection
        printf("%c", ch);
    }
}

// Set up the UART interrupt handler
void setup_uart_irq() {
    // Set up a handler for UART receive interrupts
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    // Enable the UART interrupt
    irq_set_enabled(UART0_IRQ, true);
    // Enable the UART to generate interrupts when data is received
    uart_set_irq_enables(UART_ID, true, false);
}

int main()
{
    string type;
    cin >> type;

    stdio_init_all();
    
    // Initialise the Wi-Fi chip
    // if (cyw43_arch_init()) {
    //     printf("Wi-Fi init failed\n");
    //     return -1;
    // }

 
    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    // Set the UART flow control CTS/RTS pins
    uart_set_hw_flow(UART_ID, true, true);
    // Set up the UART interrupt
    setup_uart_irq();
    // Create a thread to handle serial input

    // Detach the thread to run independently
    while (true) {
        if (tud_cdc_connected()) {
            if (tud_cdc_available()) {
                char buf[64];
                uint32_t count = tud_cdc_read(buf, sizeof(buf));
                for (uint32_t i = 0; i < count; i++) {
                    uart_putc(UART_ID, buf[i]);
                }
            }
        }
        sleep_us(10);
    }
}

