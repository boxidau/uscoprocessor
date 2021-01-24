#include "canlogger.h"

#include "clocktime.h"

#include <DebugLog.h>
#include <TimeLib.h>

static char lineBuffer[255];
static int linesWritten;
static File logFile;
static bool enableLog;

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
    char logFileName[11];
    char fullLogFilePath[19];

    sprintf(logDir, "%d%02d%02d", year(), month(), day());
    LOG_VERBOSE("Log directory", logDir);

    sprintf(logFileName, "%02d%02d%02d.csv", hour(), minute(), second());
    LOG_VERBOSE("Log file name", logFileName);

    sprintf(fullLogFilePath, "%s/%s", logDir, logFileName);
    LOG_VERBOSE("Log file full path", fullLogFilePath);

    if (!SD.exists(logDir))
    {
        if (!SD.mkdir(logDir))
        {
            LOG_ERROR("Unable to make log directory", logDir);
            return;
        }
    }
    LOG_VERBOSE("Log directory OK", logDir);

    if (SD.exists(fullLogFilePath))
    {
        LOG_ERROR("Log file already exists");
        return;
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
        LOG_WARNING("LOG DISABLED", lineBuffer);
        return;
    }
    LOG_VERBOSE("LOG WRITE:", lineBuffer);
    logFile.println(lineBuffer);
    linesWritten++;

    if (linesWritten % 50 == 0)
    {
        LOG_VERBOSE("LOG FLUSH");
        logFile.flush();
    }
}