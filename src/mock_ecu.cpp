#include <stdint.h>

#include "WProgram.h"
#include "elapsedMillis.h"

#include "FlexCAN.h"

#include "constants.h"
#include "aemnet_utils.h"
#include "aemnet_definitions.h"

FlexCAN canbus(AEMNET_BAUDRATE);
CAN_message_t msg;

bool state = 0;

using aemnet_utils::msg_00_t;

inline uint16_t swap_bytes(uint16_t a) {
    return (a >> 8) | (a << 8);
}

void setup() {
    // setup LED
    pinMode(13, OUTPUT);

    // setup CAN message
    msg.id = AEMNET_MSG_ID(0x00);
    msg.ext = 1;
    msg.len = 8;

    // setup CAN message fields
    for (int i = 0; i < 8; i++) {
        msg.buf[i] = i;
    }

    /* msg_00_t* m = (msg_00_t*)&msg.buf; */
    /* m->rpm = 3000 / RPM_SCALE; */
    /* m->rpm = swap_bytes(m->rpm); */
    /* m->coolant_temp = 98 / COOLANT_TEMP_C_SCALE; */

    canbus.begin();
}

void loop() {
    canbus.write(msg);
    digitalWrite(13, state);
    state = !state;
}

int main() {
    setup();
    elapsedMicros em;
    while (1) {
        if (em >= LOOP_PERIOD) {
            em = 0;
            loop();
        }
    }
}
