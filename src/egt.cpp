#define NDEBUG

#include "egt.h"
#include "utils.h"

#include <DebugLog.h>

uint16_t EGTClass::celsius = 0;
byte EGTClass::error = 0;

SPISettings SPISettingsEGT(4000000, MSBFIRST, SPI_MODE0);
byte rxBuffer[4];

void EGTClass::setup()
{
  pinMode(SPI_EGT_CS_PIN, OUTPUT);
  digitalWrite(SPI_EGT_CS_PIN, HIGH);
}

void EGTClass::readData()
{
  SPI.beginTransaction(SPISettingsEGT);
  digitalWriteFast(SPI_EGT_CS_PIN, LOW);
  SPI.transfer(rxBuffer, 4);
  digitalWriteFast(SPI_EGT_CS_PIN, HIGH);
  SPI.endTransaction();

  if (MOCK_DATA)
  {
    rxBuffer[0] = 0x3E;
    rxBuffer[1] = 0x80;
    rxBuffer[2] = 0x19;
  }

  LOG_VERBOSE("EGT RX", hexDump(rxBuffer));

  celsius = ((rxBuffer[0] & 0x7F) << 4 | rxBuffer[1] >> 4);
  if (rxBuffer[0] & 0x80) {
    LOG_WARNING("Truncated negative temp");
    celsius = 0;
  }

  LOG_VERBOSE("EGT Value: ", celsius);

  error = rxBuffer[3] & 0x7;  
  if (rxBuffer[3] & 0x4)
  {
    LOG_ERROR("EGT Error: Short to VCC");
    celsius = 4;
  }

  if (rxBuffer[3] & 0x2)
  {
    LOG_ERROR("EGT Error: Short to GND");
    celsius = 2;
  }

  if (rxBuffer[3] & 0x1)
  {
    LOG_ERROR("EGT Error: Open circuit");
    celsius = 1;
  }
}

void EGTClass::getCANMessage(CAN_message_t &msg) {
  EGTClass::readData();
  msg.id = EGT_CANID;
  msg.len = 3;
  msg.flags = {};
  msg.buf[0] = celsius >> 8;
  msg.buf[1] = celsius & 0xFF;
  msg.buf[2] = error;
  msg.buf[3] = 0;
  msg.buf[4] = 0;
  msg.buf[5] = 0;
  msg.buf[6] = 0;
  msg.buf[7] = 0;
}