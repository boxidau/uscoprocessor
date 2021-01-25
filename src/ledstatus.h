#define NO_ERROR        0b00000011
#define EGT_ERROR       0b00000100
#define CAN_ERROR       0b00001000
#define KNOCK_SPI_ERROR 0b00010000
#define LOGGER_ERROR    0b00100000

#include "Arduino.h"

class LEDStatus
{
public:
    static void setup();
    static void loop();

    static void setError(byte error);
    static void setError(byte error, bool setOrClear);
    static void clearError(byte error);
    static byte getError();

private:
    static byte flashCode;
    static uint8_t flashCodeIndex;
};