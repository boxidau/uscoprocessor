#include "Arduino.h"
#include <Metro.h>
#include <SPI.h>
#include <DebugLog.h>
#include <FlexCAN.h>
#include "knockdetector.h"
#include "canlogger.h"
#include "constants.h"
#include "egt.h"
#include "clocktime.h"
#include "ledstatus.h"
#include "utils.h"
#include "analogsensors.h"

Metro statsTimer = Metro(10000);
Metro pollTimer = Metro(50);

CAN_message_t egtMessage, knockMessage, analogMessage, rxMessage;
unsigned long previousLoop, loopStart;

void setup()
{
    pinMode(EXTERNAL_PWM1, OUTPUT);
    pinMode(EXTERNAL_PWM2, OUTPUT);
    digitalWrite(EXTERNAL_PWM1, LOW);
    digitalWrite(EXTERNAL_PWM2, LOW);

    pinMode(EXTERNAL_DIGITAL_PIN, INPUT);
    pinMode(EXTERNAL_DAC_1, OUTPUT);

    AnalogSensors::setup();
    LEDStatus::setup();

    Serial.begin(9600);
    LOG_VERBOSE("System Boot");

    SPI.begin();
    ClockTime::setup();
    CANLogger::setup();

    KnockDetector::setup();
    EGT::setup();

    Can0.begin(500000);

    LOG_VERBOSE("Ready");
}

void writeAndLog(CAN_message_t &message)
{
    Can0.write(message);
    CANLogger::logCANMessage(message, CAN_TX);
}

void loop()
{
    loopStart = micros();
    unsigned long loopTime = loopStart - previousLoop;
    previousLoop = loopStart;

    // tick functions for all modules
    KnockDetector::loop();
    LEDStatus::loop();
    // end tick functions

    // poll all data and write to TX_CAN log
    if (pollTimer.check())
    {
        // EGT poll and log
        EGT::getCANMessage(egtMessage);
        writeAndLog(egtMessage);
        LEDStatus::setError(EGT_ERROR, EGT::error > 0);

        // analog sensor poll and log
        AnalogSensors::getCANMessage(analogMessage);
        writeAndLog(analogMessage);
    }

    while (Can0.read(rxMessage))
    {
        CANLogger::logCANMessage(rxMessage, CAN_RX);
    }

    if (statsTimer.check())
    {
        CANLogger::logComment("loop_time=" + (String)loopTime + "uS, error_code=" + hexDump(LEDStatus::getError()));
    }
}
