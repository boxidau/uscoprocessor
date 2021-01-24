#include "Arduino.h"
#include <DebugLog.h>
#include "Print.h"

static uint8_t hex[17] = "0123456789ABCDEF";

inline String hexDump(unsigned char *arr)
{
    String out = "0x";
    for (uint8_t i = 0; i < sizeof(arr); i++)
    {
        out += (char)hex[arr[i] >> 4];
        out += (char)hex[arr[i] & 15];
    }
    return out;
}

inline String hexDump(unsigned char c)
{
    String out = "0x";
    out += (char)hex[c >> 4];
    out += (char)hex[c & 15];
    return out;
}