#include "Arduino.h"
#include <FlexCAN.h>

class AnalogSensors
{
public:
    static void setup();
    static void getCANMessage(CAN_message_t &msg);
};