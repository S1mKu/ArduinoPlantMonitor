#ifndef _SENSOR_DATA_LOG_H
#define _SENSOR_DATA_LOG_H

#include "DataLogger.h"

//#define DEFAULT_CS_PIN 10
#define DEFAULT_CS_PIN 53

#define DEFAULT_LOG_HEADER_INDEX 0
#define DEFAULT_MAX_LOG_ENTRIES 100

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
    int cur_log_file_prefix = 0;
    int data_log_header_index = 0;
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
    int _retrieve_log(LogType log);

    int write(unsigned long, const byte *, unsigned int);
    int read(unsigned long, byte *, unsigned int);

public:
    int begin(int csPin = DEFAULT_CS_PIN);
    int log(DataLogEntry entry);
    int select_last(DataLogEntry *entry);
    int select(unsigned int index, DataLogEntry entry_buffer[], unsigned int entry_buffer_len);
};

extern SensorDataLogger LOG;

#endif
