#include <stdint.h>

#include "WProgram.h"
#include "elapsedMillis.h"

#include "FlexCAN.h"

#include "constants.h"
#include "aemnet_utils.h"
#include "aemnet_definitions.h"

FlexCAN canbus0(AEMNET_BAUDRATE, 0);
FlexCAN canbus1(AEMNET_BAUDRATE, 1);

#define NUM_MSGS 5

CAN_message_t msg_00;
CAN_message_t msg_03;
CAN_message_t msg_04;
CAN_message_t msg_08;
CAN_message_t msg_09;

static uint32_t ids[NUM_MSGS] = {0x00, 0x03, 0x04, 0x08, 0x09};
CAN_message_t* msgs[NUM_MSGS] = {&msg_00, &msg_03, &msg_04, &msg_08, &msg_09};

bool state = 0;

using aemnet_utils::msg_00_t;
using aemnet_utils::fixed_point_t;

inline uint16_t swap_bytes(uint16_t a) {
    return (a >> 8) | (a << 8);
}

void init_msg(CAN_message_t* m, int id) {
    m->id = id;
    m->ext = 1;
    m->len = 8;
    m->timeout = 0;
}

void loop_msg_00(CAN_message_t* m) {
    msg_00_t* msg = (msg_00_t*)(m->buf);

    float rpm = 3000.1234;
    rpm = rpm / 0.39063;
    uint16_t rpm_int = (uint16_t)rpm;

    float throttle = 75.51;
    throttle = throttle / 0.0015259;
    uint16_t throttle_int = (uint16_t)throttle;

    float iat = 26.00;
    iat = iat / 1.0;
    int8_t iat_int = (int8_t)iat;

    float clt = 98.00;
    clt = clt / 1.0;
    int8_t clt_int = (int8_t)clt;

    msg->rpm = swap_bytes(rpm_int);
    msg->load = 0x0000;
    msg->throttle = swap_bytes(throttle_int);
    msg->intake_temp = iat_int;
    msg->coolant_temp = clt_int;
}

int counter;
void setup() {
    // setup LED
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    for (int i = 0; i < NUM_MSGS; i++) {
        init_msg(msgs[i], AEMNET_MSG_ID(ids[i]));
    }
    canbus0.begin();
    canbus1.begin();
}

void loop() {
    // setup CAN message
    loop_msg_00(&msg_00);
    canbus0.write(msg_00);
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
