#include "Arduino.h"
#include <Metro.h>
#include <SPI.h>
#include <DebugLog.h>
#include <FlexCAN.h>
#include <MegaCAN.h>

#include "knockdetector.h"
#include "canlogger.h"
#include "constants.h"
#include "egt.h"
#include "clocktime.h"
#include "ledstatus.h"
#include "utils.h"
#include "analogsensors.h"
#include "gaugeoutput.h"

Metro statsTimer = Metro(10000);
Metro pollTimer = Metro(50);

static CAN_message_t egtMessage, knockMessage, analogMessage, rxMessage;

static MegaCAN megaCAN;
static MegaCAN_broadcast_message_t megasquirtData;
const uint32_t baseID = MEGASQUIRT_BROADCAST_CANID;

unsigned long previousLoop, loopStart;

void setup()
{
    pinMode(EXTERNAL_PWM1, OUTPUT);
    pinMode(EXTERNAL_PWM2, OUTPUT);
    digitalWrite(EXTERNAL_PWM1, LOW);
    digitalWrite(EXTERNAL_PWM2, LOW);

    pinMode(EXTERNAL_DIGITAL_PIN, INPUT);

    Serial.begin(9600);
    SPI.begin();
    Can0.begin(500000);
    delay(500);
    LOG_VERBOSE("System Boot");

    ClockTime::setup();
    CANLogger::setup();

    AnalogSensors::setup();
    LEDStatus::setup();
    KnockDetector::setup();
    EGT::setup();
    GaugeOutput::setup(0, 200); // boost gauge 0-200kPa
    LOG_VERBOSE("Ready");
}

void writeAndLog(CAN_message_t &message)
{
    Can0.write(message);
    CANLogger::logCANMessage(message, CAN_TX);
}

uint16_t maptest = 0;

void processRXCANMessage()
{
    CANLogger::logCANMessage(rxMessage, CAN_RX);
    if (rxMessage.id >= MEGASQUIRT_BROADCAST_CANID && rxMessage.id <= MEGASQUIRT_BROADCAST_CANID + MEGASQUIRT_BROADCAST_PAGES) {
        megaCAN.getBCastData(rxMessage.id, rxMessage.buf, megasquirtData);
    }
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
        LEDStatus::setError(EGT_ERROR, EGT::error > 0);
        writeAndLog(egtMessage);

        // analog sensor poll and log
        AnalogSensors::getCANMessage(analogMessage);
        writeAndLog(analogMessage);

        // knock detection log
        KnockDetector::getCANMessage(knockMessage);
        LEDStatus::setError(KNOCK_SPI_ERROR, KnockDetector::error);
        writeAndLog(knockMessage);

        // Logger error status
        LEDStatus::setError(LOGGER_ERROR, CANLogger::error);

        GaugeOutput::update(megasquirtData.map);
    }


    // read canbus data if message is available
    if(Can0.read(rxMessage))
    {
        processRXCANMessage();
    }

    if (statsTimer.check())
    {
        CANLogger::logComment("loop_time=" + (String)loopTime + "uS, error_code=" + hexDump(LEDStatus::getError()));
    }
}
