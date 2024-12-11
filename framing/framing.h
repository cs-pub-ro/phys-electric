#ifndef FRAMING_H
#define FRAMING_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "../manchester_codec/sender.h"
#include "../manchester_codec/receiver.h"

class Framing {
private:
    static constexpr uint8_t DLE = 0x10;
    static constexpr uint8_t STX = 0x02;
    static constexpr uint8_t ETX = 0x03;
public:
    void send_frame(Sender& sender, const uint8_t* data, size_t length);
    int receive_frame(Receiver& receiver, uint8_t* buffer, size_t buffer_length);
};


#endif // FRAMING_H