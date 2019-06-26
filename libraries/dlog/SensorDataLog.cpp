#include "SensorDataLog.h"
#include <SPI.h>
#include <SD.h>

StatusLogEntry STATUS_LOG_ENTRY;
DataLogEntry DATA_LOG_ENTRY;

uint16_t _cs_pin;
LogType _active_log_type = NONE;

static File _log;

char logfile_name[30] = "\0";
// LogEntry log_entry;
uint32_t _log_entry_size;
uint32_t _data_log_entry_size;
uint32_t _max_log_entries;

static void print_it(unsigned long index, const uint8_t *entry_ptr, LogType type)
{
    if (index == 0) //HEADER
    {

        Serial.println("\n########## HEADER ##########");

        LogHeader const *entry = reinterpret_cast<LogHeader const *>(entry_ptr);
        Serial.print("entry_size ");
        Serial.println(entry->entry_size);
        Serial.print("flag ");
        Serial.println(entry->flag);
        Serial.print("log_size ");
        Serial.println(entry->log_size);
        Serial.print("n_entries ");
        Serial.println(entry->n_entries);

        Serial.println("########## HEADER ##########\n");
    }
    else //ENTRY
    {
        if (type == STATUS_LOG)
        {
            Serial.println("\n########## STATUS_LOG_ENTRY ##########");

            StatusLogEntry const *entry = reinterpret_cast<StatusLogEntry const *>(entry_ptr);
            Serial.print("prefix ");
            Serial.println(entry->cur_log_file_prefix);
            Serial.print("header_index ");
            Serial.println(entry->data_log_header_index);

            Serial.println("########## STATUS_LOG_ENTRY ##########\n");
        }
        else
        {
            Serial.println("\n########## DATA_LOG_ENTRY ##########");

            DataLogEntry const *entry = reinterpret_cast<DataLogEntry const *>(entry_ptr);
            Serial.println(entry->brightness);
            Serial.println(entry->humidity);
            Serial.println(entry->moisture);
            Serial.println(entry->temperature);

            Serial.println("########## DATA_LOG_ENTRY ##########\n");
        }
    }
}

int16_t SensorDataLogger::_retrieve_log(LogType log)
{
    if (log == NONE || log == _active_log_type)
    {
        return 0;
    }

    logfile_name[0] = '\0';
    //strcat(logfile_name, "/");

    if (_active_log_type != log && _active_log_type != NONE)
    {
        Serial.println("Closing file");
        _log.close();
    }

    delay(200);

    if (log == STATUS_LOG)
    {
        _active_log_type = STATUS_LOG;
        strcat(logfile_name, STATUS_LOG_FILE);
        // log_entry = LOG_ENTRY STATUS_LOG_ENTRY;
        _log_entry_size = sizeof(STATUS_LOG_ENTRY);

        // Serial.print("LOG::_retrieve_log - STATUS_LOG_FILE ");
        // Serial.println(logfile_name);
    }
    else if (log == DATA_LOG)
    {
        _active_log_type = DATA_LOG;

        // if (!SD.exists(DATA_LOG_BASE_DIR))
        // {
        //     //Serial.println("LOG::_retrieve_log - ERROR 0");
        //     if (!SD.mkdir(DATA_LOG_BASE_DIR))
        //     {
        //         return -1;
        //     }
        // }

        //sprintf(logfile_name, "%s%d", DATA_LOG_BASE_DIR, STATUS_LOG_ENTRY.cur_log_file_prefix);
        //strcat(logfile_name, DEFAULT_LOG_FILE_SUFFIX);

        sprintf(logfile_name, "%s%d%s", "D", STATUS_LOG_ENTRY.cur_log_file_prefix, DEFAULT_LOG_FILE_SUFFIX);

        // log_entry = LOG_ENTRY DATA_LOG_ENTRY;
        _log_entry_size = _data_log_entry_size;

        //Serial.print("LOG::_retrieve_log - DATA_LOG_FILE ");
        //Serial.println(logfile_name);
    }
    else
    {
        //Serial.println("LOG::_retrieve_log - ERROR 1");
        return -2;
    }

    delay(200);

    bool exists = SD.exists(logfile_name);

    //_log = SD.open(logfile_name, FILE_WRITE);
    _log = SD.open(logfile_name, (O_READ | O_WRITE | O_CREAT));

    delay(200);

    Serial.println(SD.exists(logfile_name));

    Serial.println(logfile_name);
    Serial.println(_log_entry_size);

    Serial.print("Opened file ");
    Serial.println(_log.name());
    Serial.println(_log);
    Serial.print("Log file existed before: ");
    Serial.println(exists);

    delay(200);

    if (_log)
    {
        if (exists)
        {
            //Serial.println("LOG::_retrieve_log - trying to open log");
            LogStatus status;
            if ((status = openLog(DEFAULT_LOG_HEADER_INDEX)) != LOG_OK)
            {
                //Serial.print("_retrieve_log() :: ERROR 2 - status ");
                //Serial.println(status);
                return -3;
            }
        }
        else // log does not exist yet
        {
            //Serial.println("LOG::_retrieve_log - trying to create log");
            if (createLog(DEFAULT_LOG_HEADER_INDEX, _max_log_entries, _log_entry_size) != LOG_OK)
            {
                //Serial.println("LOG::_retrieve_log :: ERROR 3");
                return -4;
            }
        }
    }
    else
    {
        //Serial.println("LOG::_retrieve_log - ERROR 4");
        return -5;
    }

    // Serial.println("LOG::_retrieve_log - returning true");
    delay(200);

    Serial.println(_log.name());
    Serial.println("LOG::_retrieve_log - exiting");

    return 0;
}

int16_t SensorDataLogger::begin(uint32_t entry_size, uint32_t max_log_entries = DEFAULT_MAX_LOG_ENTRIES, uint16_t cs_pin = DEFAULT_CS_PIN)
{
    _data_log_entry_size = entry_size;
    _max_log_entries = max_log_entries;
    uint32_t err_no = 0;
    _cs_pin = cs_pin;

    Serial.println("LOG::begin");
    delay(100);

    if (!SD.begin(cs_pin))
    {
        Serial.println("No SD-card.");
        return -100;
    }

    if ((err_no = _retrieve_log(STATUS_LOG)) != 0)
    {
        Serial.println(err_no);
        return -6;
    }

    if (countEntries()) //at least one state entry
    {
        if (readEntry(1, LOG_ENTRY STATUS_LOG_ENTRY) != LOG_OK)
        {
            return -7;
        }
    }
    else //no state entry yet
    {
        Serial.println("----");
        if (appendEntry(LOG_ENTRY STATUS_LOG_ENTRY) != LOG_OK)
        {
            return -8;
        }
    }
    delay(200);

    Serial.println("exiting");

    return 0;
}

int16_t SensorDataLogger::write(uint32_t index, const uint8_t *entry_ptr, uint16_t entry_len)
{
    Serial.println("LOG::write() - entered ");
    delay(200);

    // print_it(index, entry_ptr, _active_log_type);

    Serial.print("########## entry_len: ");
    Serial.println(entry_len);
    Serial.print("########## Startsize: ");
    Serial.println(_log.size());
    Serial.print("########## Startposition: ");
    Serial.println(_log.position());

    if (!_log.seek(index))
    {
        return -1;
    }

    Serial.print("########## Writing to position: ");
    Serial.println(_log.position());

    int16_t len = _log.write(entry_ptr, entry_len);
    _log.flush();

    Serial.print("########## Endsize: ");
    Serial.println(_log.size());
    Serial.print("########## Endposition: ");
    Serial.println(_log.position());

    delay(200);
    return len;
}

int16_t SensorDataLogger::read(unsigned long index, uint8_t *entry_ptr, uint16_t entry_len)
{
    Serial.print("LOG::read() - entered ");
    Serial.println(entry_len);

    if (!_log.seek(index))
    {
        return -1;
    }
    int16_t res = _log.read(entry_ptr, entry_len);
    _log.flush();

    Serial.print(" -----> ");
    Serial.println(_active_log_type);

    // print_it(index, entry_ptr, _active_log_type);

    delay(200);
    return res;
}

int16_t SensorDataLogger::log(const LogEntry entry)
{
    int16_t err_no = 0;

    Serial.println("LOG::log 1");

    if ((err_no = _retrieve_log(DATA_LOG)) != 0)
    {
        Serial.println(err_no);
        return -9;
    }
    delay(200);

    // print_it(1, entry, _active_log_type);

    if (appendEntry(entry) == LOG_FULL)
    {
        Serial.println("LOG::log 2");
        if ((err_no = _retrieve_log(STATUS_LOG)) != 0)
        {
            Serial.println(err_no);
            return -10;
        }

        STATUS_LOG_ENTRY.cur_log_file_prefix++;

        Serial.println("LOG::log 3");
        Serial.println(log_header.n_entries);

        if (updateEntry(1, LOG_ENTRY STATUS_LOG_ENTRY) != LOG_OK)
        {
            return -11;
        }

        if (!log(entry))
        {
            return -12;
        }
    }
    delay(200);

    return 0;
}

int16_t SensorDataLogger::select_last(LogEntry entry)
{
    // int16_t err_no = 0;

    // Serial.println("LOG::select_last - entered");

    // if ((err_no = select(log_header.n_entries, entry, 1)) != 0)
    // {
    //     Serial.println(err_no);
    //     return -16;
    // }

    // delay(200);
    // return 0;

    int16_t err_no = 0;

    Serial.println("LOG::select_last - entered");

    if ((err_no = _retrieve_log(DATA_LOG)) != 0)
    {
        Serial.println(err_no);
        return -13;
    }

    // if (readEntry(log_header.n_entries, LOG_ENTRY(*entry)) != LOG_OK)
    // {
    //     return -14;
    // }

    if (readEntry(log_header.n_entries, entry) != LOG_OK)
    {
        return -14;
    }

    delay(200);
    return 0;
}

int16_t SensorDataLogger::select(uint16_t index, LogEntry entry_buffer[], uint16_t entry_buffer_len)
{
    int16_t err_no = 0;

    Serial.println("LOG::select - entered");

    if (log_header.n_entries < index - 1 + entry_buffer_len)
    {
        return -17;
    }

    if ((err_no = _retrieve_log(DATA_LOG)) != 0)
    {
        Serial.println(err_no);
        return -13;
    }

    int16_t i;
    for (i = 0; i < entry_buffer_len; i++)
    {
        if (readEntry(index + i, entry_buffer[i]) != LOG_OK)
        {
            return -14;
        }
    }

    delay(200);
    return 0;
}

int16_t SensorDataLogger::count_entries(void)
{
    return countEntries();
}

SensorDataLogger LOG = SensorDataLogger();
