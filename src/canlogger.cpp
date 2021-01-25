/*
Writes CRTD log files for CANBUS logging
https://docs.openvehicles.com/en/latest/crtd
*/

#include "canlogger.h"

#include "clocktime.h"

#include <DebugLog.h>
#include <Metro.h>
#include <TimeLib.h>

static char lineBuffer[255];
static int linesWritten;
static File logFile;
static bool enableLog;
static Metro retrySDTimer = Metro(1000);

bool CANLogger::error = false;

void CANLogger::setup()
{
    linesWritten = 0;

    LOG_VERBOSE("Initializing SD card");
    if (!SD.begin(BUILTIN_SDCARD))
    {
        LOG_VERBOSE("SD card initialization failed, is a card inserted?");
        return;
    }

    LOG_VERBOSE("An SD card is present");

    char logDir[9];
    char logFileName[13];
    char fullLogFilePath[19];

    sprintf(logDir, "%d%02d%02d", year(), month(), day());
    if (year() < 1980) // rtc is not set
    {
        sprintf(logDir, "%s", "NODATE");
    }
    if (!SD.exists(logDir))
    {
        if (!SD.mkdir(logDir))
        {
            LOG_ERROR("Unable to make log directory", logDir);
            return;
        }
    }
    LOG_VERBOSE("Log directory OK", logDir);


    sprintf(logFileName, "%02d%02d%02d.crd", hour(), minute(), second());
    if (year() < 1980) // rtc is not set
    {
        File noDateDirectory = SD.open(logDir);
        File existingLogFile;
        int maxLog = 0;
        while (existingLogFile = noDateDirectory.openNextFile()) {
            int curLog = 0;
            if(sscanf(existingLogFile.name(), "%d.crd", &curLog) && curLog > maxLog) {
                maxLog = curLog;
            }
        }
        noDateDirectory.close();
        sprintf(logFileName, "%08d.crd", maxLog + 1);   
    }

    LOG_VERBOSE("Log directory", logDir);
    LOG_VERBOSE("Log file name", logFileName);
    sprintf(fullLogFilePath, "%s/%s", logDir, logFileName);
    LOG_VERBOSE("Log file full path", fullLogFilePath);


    if (SD.exists(fullLogFilePath))
    {
        LOG_ERROR("Log file already exists, overwriting");
    }

    logFile = SD.open(fullLogFilePath, FILE_WRITE);
    enableLog = true;
}

void CANLogger::logComment(const String line)
{
    char strBuf[241];
    line.toCharArray(strBuf, 240);

    sprintf(lineBuffer, "%0.3f CXX %s", ClockTime::getMillisTime(), strBuf);
    write();
}

void CANLogger::logCANMessage(const CAN_message_t &message, bool rx)
{
    sprintf(lineBuffer, "%0.3f %d%s%s %lu %02X %02X %02X %02X %02X %02X %02X %02X",
            ClockTime::getMillisTime(),
            1, // hardcode canbus 1 for now
            rx ? "R" : "T",
            message.flags.extended ? "29" : "11",
            message.id,
            message.buf[0],
            message.buf[1],
            message.buf[2],
            message.buf[3],
            message.buf[4],
            message.buf[5],
            message.buf[6],
            message.buf[7]);
    write();
}

void CANLogger::write()
{
    if (!enableLog)
    {
        if (random(0, 20) == 0) LOG_WARNING("LOG DISABLED", lineBuffer);
        if (retrySDTimer.check())
        {
            CANLogger::setup();
        }
        return;
    }

    if (random(0, 20) == 0) LOG_VERBOSE("LOG WRITE:", lineBuffer);

    if (logFile.print(lineBuffer) != strlen(lineBuffer))
    {
        enableLog = false;
        error = true;
        return;
    }
    
    error = false;
    if (++linesWritten % 50 == 0)
    {
        LOG_VERBOSE("LOG FLUSH, written lines: ", linesWritten);
        logFile.flush();
    }
}