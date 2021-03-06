#include "Arduino.h"

#include <SPI.h>
#include "constants.h"

#include <FlexCAN.h>

class EGT
{
public:
    static uint16_t celsius;
    static byte error;
    static void setup();
    static void getCANMessage(CAN_message_t &msg);
    static void readData();
};
