#include "analogsensors.h"
#include "constants.h"
#include <Metro.h>
#include <DebugLog.h>



void AnalogSensors::setup() {
    pinMode(EXTERNAL_ADC1, INPUT);
    pinMode(EXTERNAL_ADC2, INPUT);
    pinMode(EXTERNAL_NTC1, INPUT);
    pinMode(EXTERNAL_NTC2, INPUT);
}

void AnalogSensors::getCANMessage(CAN_message_t &msg) {

    analogReadRes(12);
    uint16_t adc1_val = analogRead(EXTERNAL_ADC1);
    uint16_t adc2_val = analogRead(EXTERNAL_ADC2);
    uint16_t ntc1_val = analogRead(EXTERNAL_NTC1);
    uint16_t ntc2_val = analogRead(EXTERNAL_NTC2);

    msg.id = ANALOG_CANID;
    msg.len = 8;
    msg.flags = {};

    msg.buf[0] = adc1_val >> 8;
    msg.buf[1] = adc1_val & 0xFF;

    msg.buf[2] = adc2_val >> 8;
    msg.buf[3] = adc2_val & 0xFF;

    msg.buf[4] = ntc1_val >> 8;
    msg.buf[5] = ntc1_val & 0xFF;

    msg.buf[6] = ntc2_val >> 8;
    msg.buf[7] = ntc2_val & 0xFF;
}