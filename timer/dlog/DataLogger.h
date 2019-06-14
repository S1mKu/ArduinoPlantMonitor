#ifndef _DATA_LOGGER_H_
#define _DATA_LOGGER_H_

#include <Arduino.h>

#define LOG_FLAG B11011011

struct LogHeader
{
    byte flag;
    unsigned long n_entries;
    unsigned long entry_size;
    unsigned long log_size;
};
extern LogHeader log_header;

enum LogStatus
{
    LOG_OK,
    LOG_ERROR,
    LOG_OUT_OF_RANGE,
    LOG_FULL
};

typedef byte *LogEntry;

#define LOG_ENTRY (byte *)(void *) &

class BaseDataLogger
{
public:
    virtual int write(unsigned long, const byte *, unsigned int) = 0;
    virtual int read(unsigned long, byte *, unsigned int) = 0;

    LogStatus createLog(unsigned long, unsigned long, unsigned long);
    LogStatus openLog(unsigned long);
    LogStatus readEntry(unsigned long, LogEntry);
    LogStatus writeEntry(unsigned long, const LogEntry);
    LogStatus deleteEntry(unsigned long);
    LogStatus insertEntry(unsigned long, const LogEntry);
    LogStatus updateEntry(unsigned long, const LogEntry);
    LogStatus appendEntry(const LogEntry);
    unsigned long maxEntries();
    unsigned long countEntries();
    void clearEntries();

private:
    void writeHead();
    void readHead();
};

#endif
