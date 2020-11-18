#if 0
#include <stdint.h>

#include "WProgram.h"
#include "elapsedMillis.h"

#include "FlexCAN.h"

#include "constants.h"
#include "aemnet_utils.h"
#include "aemnet_definitions.h"

FlexCAN canbus(AEMNET_BAUDRATE);
FlexCAN canbus2(AEMNET_BAUDRATE, 1);

CAN_message_t tx_msg;
CAN_message_t rx_msg;

CAN_message_t tx_msg2;
CAN_message_t rx_msg2;

bool state = 0;

using aemnet_utils::msg_00_t;

inline uint16_t swap_bytes(uint16_t a) {
    return (a >> 8) | (a << 8);
}

int counter;
void setup() {
    // setup LED
    pinMode(13, OUTPUT);

    counter = 0;

    /* msg_00_t* m = (msg_00_t*)&msg.buf; */
    /* m->rpm = 3000 / RPM_SCALE; */
    /* m->rpm = swap_bytes(m->rpm); */
    /* m->coolant_temp = 98 / COOLANT_TEMP_C_SCALE; */

    Serial.begin(115200);
    canbus.begin();
    canbus2.begin();
}

void loop() {
    Serial.print("loop start: ");
    Serial.println(counter);

    while (canbus.read(rx_msg)) {
        for (int i = 0; i < 8; i++) {
            Serial.print(rx_msg.buf[i]);
            Serial.print(", ");
        }
        Serial.println("");
        Serial.println("can0 received msg");
    }

    while (canbus2.read(rx_msg2)) {
        for (int i = 0; i < 8; i++) {
            Serial.print(rx_msg2.buf[i]);
            Serial.print(", ");
        }
        Serial.println("");
        Serial.println("can1 received msg");
    }

    // setup CAN message
    tx_msg.id = AEMNET_MSG_ID(0x100);
    tx_msg.ext = 1;
    tx_msg.len = 8;
    tx_msg.timeout = 0;
    for (int i = 0; i < 8; i++) {
        tx_msg.buf[i] = i;
    }
    canbus.write(tx_msg);
    Serial.println("can0 sent msg");

    // setup CAN message
    tx_msg2.id = AEMNET_MSG_ID(0x101);
    tx_msg2.ext = 1;
    tx_msg2.len = 8;
    tx_msg2.timeout = 0;
    for (int i = 0; i < 8; i++) {
        tx_msg2.buf[i] = 7-i;
    }
    canbus2.write(tx_msg2);
    Serial.println("can1 sent msg");

    digitalWrite(13, state);
    state = !state;

    Serial.print("loop end: ");
    Serial.println(counter++);
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
#endif
