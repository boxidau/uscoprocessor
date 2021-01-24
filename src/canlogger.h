#include "Arduino.h"
#include <FlexCAN.h>
#include <SD.h>

#define CAN_RX true
#define CAN_TX false

class CANLogger
{
public:
    static void setup();
    static void logComment(const String line);
    static void logCANMessage(const CAN_message_t &message, bool rx);

private:
    static void write();
};