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
using aemnet_utils::msg_03_t;
using aemnet_utils::msg_04_t;
using aemnet_utils::msg_08_t;
using aemnet_utils::msg_09_t;
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

uint8_t float2fixed_u8(float val, float scale, float offset) {
    val -= offset;
    val /= scale;
    return (uint8_t)val;
}

int8_t float2fixed_s8(float val, float scale, float offset) {
    val -= offset;
    val /= scale;
    return (int8_t)val;
}

uint16_t float2fixed_u16(float val, float scale, float offset) {
    val -= offset;
    val /= scale;
    return (uint16_t)val;
}

void loop_msg_00(CAN_message_t* m) {
    msg_00_t* msg = (msg_00_t*)(m->buf);
    msg->rpm = swap_bytes(float2fixed_u16(3000.1234, 0.39063, 0.0));
    msg->load = 0x0000;
    msg->throttle = swap_bytes(float2fixed_u16(75.51, 0.0015259, 0.0));
    msg->intake_temp = float2fixed_s8(26.00, 1.0, 0.0);
    msg->coolant_temp = float2fixed_s8(90.00, 1.0, 0.0);
}

void loop_msg_03(CAN_message_t* m) {
    msg_03_t* msg = (msg_03_t*)(m->buf);
    msg->afr1 = float2fixed_u8(14.7, 0.057227, 7.325);
    msg->afr2 = float2fixed_u8(13.5, 0.057227, 7.325);
    msg->vehicle_speed = swap_bytes(float2fixed_u16(35.0, 0.00390625, 0.0));
    msg->gear = 2;
    msg->ign_timing = float2fixed_u8(10.0, 0.35156, -17);
    msg->battery_voltage = swap_bytes(float2fixed_u16(13.7, 0.0002455, 0.0));
}

void loop_msg_04(CAN_message_t* m) {
    msg_04_t* msg = (msg_04_t*)(m->buf);
    msg->manifold_pressure = swap_bytes(float2fixed_u16(-10.0, 0.014504, -14.6960));
    msg->ve = 75;
    msg->fuel_pressure = float2fixed_u8(45.0, 0.580151, 0.0);
    msg->oil_pressure = float2fixed_u8(80.0, 0.580151, 0.0);
    msg->afr_target = float2fixed_u8(13.5, 0.057227, 7.325);
    msg->bitmap0 = 0b00010011;
    msg->bitmap1 = 0b00000000;
}

int counter;
void setup() {
    // setup LED
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    counter = 0;

    for (int i = 0; i < NUM_MSGS; i++) {
        init_msg(msgs[i], AEMNET_MSG_ID(ids[i]));
    }
    canbus0.begin();
    canbus1.begin();
}

void loop() {
    // setup CAN message
    canbus0.write(msg_00);
    canbus0.write(msg_03);
    canbus0.write(msg_04);
    loop_msg_00(&msg_00);
    loop_msg_03(&msg_03);
    loop_msg_04(&msg_04);
    counter++;
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
