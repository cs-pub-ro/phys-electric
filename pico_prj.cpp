#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "tusb.h"
#include "framing/framing.h"
#include <string>
#include <iostream>

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define BAUD_RATE 9600

#define TX_PIN 12
#define RX_PIN 13

#define CLOCK_PERIOD_US 100

using namespace std;

int main()
{
    string type;
    type = "sender";

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

 
    // // Set up our UART
    // uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    //gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    //gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // // Read a     character from UART and echo it back
    // if (uart_is_readable(UART_ID)) {
    //     char ch = uart_getc(UART_ID);
    //     //uart_putc(UART_ID, ch);
    //     printf("%c", ch);
    // }

    if (type == "sender") {
        Sender sender;
        sender.init(TX_PIN, BAUD_RATE, CLOCK_PERIOD_US);

        // Send a frame
        uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
        Framing framing;
        framing.send_frame(sender, data, sizeof(data));
        
    } else if (type == "receiver") {
        Receiver receiver;
        receiver.init(RX_PIN, BAUD_RATE, CLOCK_PERIOD_US);

        // Receive a frame
        uint8_t buffer[4];
        Framing framing;
        framing.receive_frame(receiver, buffer, sizeof(buffer));

    } else {
        printf("Invalid type. Choose between \"sender\" and \"receiver\" \n");
        return -1;
    }
}