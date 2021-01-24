#include "clocktime.h"

#include <DebugLog.h>

static time_t initialTime = 0;

static time_t getTeensy3Time()
{
    return Teensy3Clock.get();
}

void ClockTime::setup()
{
    setSyncProvider(getTeensy3Time);
    if (timeStatus() != timeSet)
    {
        LOG_ERROR("Unable to sync with the RTC");
    }
    else
    {
        LOG_VERBOSE("RTC has set the system time");
    }
    initialTime = now();
}

double ClockTime::getMillisTime()
{
    return (double)initialTime + ((double)millis() / 1000);
}
