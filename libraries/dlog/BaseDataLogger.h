#ifndef _DATA_LOGGER_H_
#define _DATA_LOGGER_H_

#include <Arduino.h>
#include <stdint.h>

#define LOG_FLAG B11011011

struct LogHeader
{
    uint8_t flag;
    uint32_t n_entries;
    uint32_t entry_size;
    uint32_t log_size;
};
extern LogHeader log_header;

enum LogStatus
{
    LOG_OK,
    LOG_ERROR,
    LOG_OUT_OF_RANGE,
    LOG_FULL
};

typedef uint8_t *LogEntry;

#define LOG_ENTRY (uint8_t *)(void *) &

class BaseDataLogger
{
public:
    virtual int16_t write(uint32_t, const uint8_t *, uint16_t) = 0;
    virtual int16_t read(uint32_t, uint8_t *, uint16_t) = 0;

    LogStatus createLog(uint32_t, uint32_t, uint32_t);
    LogStatus openLog(uint32_t);
    LogStatus readEntry(uint32_t, LogEntry);
    LogStatus writeEntry(uint32_t, const LogEntry);
    LogStatus deleteEntry(uint32_t);
    LogStatus insertEntry(uint32_t, const LogEntry);
    LogStatus updateEntry(uint32_t, const LogEntry);
    LogStatus appendEntry(const LogEntry);
    uint32_t maxEntries();
    uint32_t countEntries();
    void clearEntries();

private:
    void writeHead();
    void readHead();
};

#endif
