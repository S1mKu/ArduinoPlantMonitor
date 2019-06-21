#ifndef _SENSOR_DATA_LOG_H
#define _SENSOR_DATA_LOG_H

#include "BaseDataLogger.h"

//#define DEFAULT_CS_PIN 10
#define DEFAULT_CS_PIN 53

#define DEFAULT_LOG_HEADER_INDEX 0
#define DEFAULT_MAX_LOG_ENTRIES 1000

#define DEFAULT_LOG_FILE_PREFIX 0
#define DEFAULT_LOG_FILE_SUFFIX ".TXT"
#define DEFAULT_DATA_LOG_FILE_NAME "D0.TXT"
#define STATUS_LOG_FILE "stat.txt"

enum LogType
{
    STATUS_LOG,
    DATA_LOG,
    NONE
};

struct StatusLogEntry
{
    uint16_t cur_log_file_prefix = 0;
    uint16_t data_log_header_index = 0;
};
extern StatusLogEntry STATUS_LOG_ENTRY;

struct DataLogEntry
{
    byte moisture = 21;
    byte humidity = 22;
    byte brightness = 23;
    byte temperature = 24;
};
extern DataLogEntry DATA_LOG_ENTRY;

class SensorDataLogger : protected BaseDataLogger
{
private:
    int16_t _retrieve_log(LogType log);

    int16_t write(uint32_t, const uint8_t *, uint16_t);
    int16_t read(uint32_t, uint8_t *, uint16_t);

public:
    int16_t begin(uint32_t entry_size, uint32_t max_log_entries = DEFAULT_MAX_LOG_ENTRIES, uint16_t csPin = DEFAULT_CS_PIN);
    int16_t log(const LogEntry entry);
    int16_t select_last(LogEntry entry);
    int16_t select(uint16_t index, LogEntry entry_buffer[], uint16_t entry_buffer_len);
    int16_t count_entries(void);
};

extern SensorDataLogger LOG;

#endif
