#include "clocktime.h"

#include <DebugLog.h>

time_t initialTime = 0;

time_t getTeensy3Time() {
    return Teensy3Clock.get();
}

void ClockTimeClass::setup() {
    setSyncProvider(getTeensy3Time);      
    if (timeStatus() != timeSet) {
        LOG_ERROR("Unable to sync with the RTC");
    } else {
        LOG_VERBOSE("RTC has set the system time");
    }
    initialTime = now();
}

double ClockTimeClass::getMillisTime() {
    return (double)initialTime + ((double)millis() / 1000);
}

