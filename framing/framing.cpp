#include "framing.h"

void Framing::send_frame(Sender& sender, const uint8_t* data, size_t length) {
    sender.send_manchester_byte(DLE);
    sender.send_manchester_byte(STX);

    for (size_t i = 0; i < length; ++i) {
        if (data[i] == DLE) {
            sender.send_manchester_byte(DLE);
        }
        sender.send_manchester_byte(data[i]);
    }

    sender.send_manchester_byte(DLE);
    sender.send_manchester_byte(ETX);
}

int Framing::receive_frame(Receiver& receiver, uint8_t* buffer, size_t buffer_length) {
uint8_t c1 = receiver.receive_manchester_byte();
    uint8_t c2 = receiver.receive_manchester_byte();

    while (!((c1 == DLE) && (c2 == STX))) {
        c1 = c2;
        c2 = receiver.receive_manchester_byte();
    }

    for (size_t i = 0; i < buffer_length; ++i) {
        uint8_t byte = receiver.receive_manchester_byte();

        if (byte == DLE) {
            byte = receiver.receive_manchester_byte();

            if (byte == ETX) {
                return i;
            } else if (byte != DLE) {
                return -1;
            }
        }

        buffer[i] = byte;
    }

    return -1;
}