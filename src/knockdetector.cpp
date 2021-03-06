#define NDEBUG
#include "Arduino.h"
#include "knockdetector.h"
#include "constants.h"

#include <DebugLog.h>
#include <SPI.h>
#include "utils.h"
#include "ledstatus.h"

#include <Metro.h>

static volatile uint16_t knockValue = 0;
static volatile unsigned long tachPeriod = 0x8FFF;
static unsigned long previousTach = 0;
static uint8_t timeConstant = KNOCK_MAX_INTEG_TIME;
static uint16_t engineSpeed = 0;
static Metro debugTimer = Metro(1000);
static SPISettings SPISettingsKnock(3000000, MSBFIRST, SPI_MODE1);

bool KnockDetector::error = false;

void TachInterrupt()
{
    // measurement window ends
    digitalWriteFast(KNOCK_HOLD_PIN, LOW);

    // Calculate period between tach interrupts
    tachPeriod = micros() - previousTach;
    previousTach = micros();

    knockValue = analogRead(KNOCK_ADC_INPUT_PIN);
    digitalWriteFast(KNOCK_HOLD_PIN, HIGH);
    digitalWriteFast(TACH_LED, !digitalReadFast(TACH_LED));
}

void KnockDetector::setup()
{
    pinMode(SPI_KNOCK_CS_PIN, OUTPUT);
    digitalWrite(SPI_KNOCK_CS_PIN, HIGH);

    pinMode(KNOCK_TEST_PIN, OUTPUT);
    digitalWrite(KNOCK_TEST_PIN, HIGH);

    pinMode(KNOCK_HOLD_PIN, OUTPUT);
    digitalWrite(KNOCK_HOLD_PIN, LOW);

    pinMode(TACH_INPUT_PIN, INPUT);

    LOG_VERBOSE("Set prescaler");
    SPIWrite(KNOCK_SET_PRESCALER);

    LOG_VERBOSE("Set set channel 1");
    SPIWrite(KNOCK_SET_CHANNEL_1);

    LOG_VERBOSE("Set band pass");
    SPIWrite(KNOCK_SET_BAND_PASS_FREQ);

    LOG_VERBOSE("Set set gain");
    SPIWrite(KNOCK_SET_GAIN);

    LOG_VERBOSE("Set integration time");
    SPIWrite(KNOCK_MIN_INTEG_TIME);

    pinMode(EXTERNAL_DAC_0, OUTPUT);
    pinMode(TACH_LED, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(TACH_INPUT_PIN), TachInterrupt, RISING);
}

char KnockDetector::SPIWrite(const char tx_data)
{
    SPI.beginTransaction(SPISettingsKnock);
    digitalWriteFast(SPI_KNOCK_CS_PIN, LOW);
    byte rx_data = 0x00;
    rx_data = SPI.transfer(tx_data);
    digitalWriteFast(SPI_KNOCK_CS_PIN, HIGH);
    SPI.endTransaction();

    LOG_VERBOSE("Knock SPI TX/RX", hexDump(tx_data), hexDump(rx_data));

    return rx_data;
}

void KnockDetector::loop()
{
    static uint16_t previousEngineSpeed;
    engineSpeed = constrain(
        (int)(60000 / (tachPeriod * NUMBER_OF_CYLINDERS / 2000)),
        MIN_RPM,
        MAX_RPM);

    if (debugTimer.check())
    {
        LOG_VERBOSE("RPM", engineSpeed);
        LOG_VERBOSE("Knock time constant", hexDump(timeConstant));
        LOG_VERBOSE("Knock ADC", knockValue);
        LOG_VERBOSE("Knock %", knockValue / 1023 * 100);
        LOG_VERBOSE("Tach Period", tachPeriod);
    }

    analogWriteDAC0(map(knockValue, 0, 1023, 0, 4095)); // max val 4059 (12bit)

    // LOG_VERBOSE(knockValue);
    if (abs(engineSpeed - previousEngineSpeed) < 50)
    {
        return; // early abort no need to update integration time, it hasn't changed much
    }
    previousEngineSpeed = engineSpeed;

    // map time constant in range based on RPM
    timeConstant = map(engineSpeed, MIN_RPM, MAX_RPM, KNOCK_MAX_INTEG_TIME, KNOCK_MIN_INTEG_TIME);

    if (SPIWrite(timeConstant) != timeConstant)
    {
        LOG_ERROR("INVALID RESPONSE FROM TPIC");
        error = true;
        LEDStatus::setError(KNOCK_SPI_ERROR, true);
    }
    else
    {
        error = false;
        LEDStatus::setError(KNOCK_SPI_ERROR, false);
    };
}

void KnockDetector::getCANMessage(CAN_message_t &msg)
{
    msg.id = KNOCK_CANID;
    msg.len = 6;
    msg.flags = {};

    msg.buf[0] = knockValue >> 8;
    msg.buf[1] = knockValue & 0xFF;

    msg.buf[2] = engineSpeed >> 8;
    msg.buf[3] = engineSpeed & 0xFF;

    msg.buf[4] = timeConstant;
    msg.buf[5] = error;
}
