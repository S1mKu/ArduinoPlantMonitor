#include "DataLogger.h"

LogHeader log_header;

unsigned long log_header_index;
unsigned long first_entry_index;

void BaseDataLogger::writeHead()
{
    Serial.println("\n########## header ##########");
    Serial.print("entry_size ");
    Serial.println(log_header.entry_size);
    Serial.print("flag ");
    Serial.println(log_header.flag);
    Serial.print("log_size ");
    Serial.println(log_header.log_size);
    Serial.print("n_entries ");
    Serial.println(log_header.n_entries);
    Serial.println(log_header_index);

    write(log_header_index, LOG_ENTRY log_header, (unsigned long)sizeof(LogHeader));

    Serial.println("########## header ##########\n");
}

void BaseDataLogger::readHead()
{
    read(log_header_index, LOG_ENTRY log_header, (unsigned long)sizeof(LogHeader));
}

LogStatus BaseDataLogger::createLog(unsigned long header_index, unsigned long tablesize, unsigned long entrysize)
{
    log_header_index = header_index;
    first_entry_index = sizeof(LogHeader) + log_header_index;
    log_header.flag = LOG_FLAG;
    log_header.n_entries = 0;
    log_header.entry_size = entrysize;
    log_header.log_size = tablesize;

    // Serial.println(log_header_index);
    // Serial.println(log_header.flag);
    // Serial.println(log_header.n_entries);
    // Serial.println(log_header.log_size);
    // Serial.println(log_header.entry_size);

    writeHead();
    if (log_header.flag == LOG_FLAG)
    {
        return LOG_OK;
    }
    else
    {
        return LOG_ERROR;
    }
}

LogStatus BaseDataLogger::openLog(unsigned long header_index)
{
    log_header_index = header_index;
    first_entry_index = sizeof(LogHeader) + log_header_index;
    readHead();
    return LOG_OK;
}

LogStatus BaseDataLogger::writeEntry(unsigned long entry_no, const LogEntry entry)
{
    write(first_entry_index + ((entry_no - 1) * log_header.entry_size), entry, log_header.entry_size);
    return LOG_OK;
}

LogStatus BaseDataLogger::readEntry(unsigned long entry_no, LogEntry entry)
{
    if (entry_no < 1 || entry_no > log_header.n_entries)
        return LOG_OUT_OF_RANGE;
    read(first_entry_index + ((entry_no - 1) * log_header.entry_size), entry, log_header.entry_size);
    return LOG_OK;
}

LogStatus BaseDataLogger::deleteEntry(unsigned long entry_no)
{
    if (entry_no < 0 || entry_no > log_header.n_entries)
        return LOG_OUT_OF_RANGE;
    LogEntry entry = (byte *)malloc(log_header.entry_size);
    for (unsigned long i = entry_no + 1; i <= log_header.n_entries; i++)
    {
        readEntry(i, entry);
        writeEntry(i - 1, entry);
    }
    free(entry);
    log_header.n_entries--;
    writeHead();
    return LOG_OK;
}

LogStatus BaseDataLogger::insertEntry(unsigned long entry_no, LogEntry entry)
{
    if (countEntries() == maxEntries())
        return LOG_FULL;
    if (countEntries() > 0 && (entry_no < 0 || entry_no > log_header.n_entries))
        return LOG_OUT_OF_RANGE;
    if (countEntries() == 0 && entry_no == 1)
        return appendEntry(entry);

    LogEntry buf = (byte *)malloc(log_header.entry_size);
    for (unsigned long i = log_header.n_entries; i >= entry_no; i--)
    {
        readEntry(i, buf);
        writeEntry(i + 1, buf);
    }
    free(buf);
    writeEntry(entry_no, entry);
    log_header.n_entries++;
    writeHead();
    return LOG_OK;
}

LogStatus BaseDataLogger::updateEntry(unsigned long entry_no, LogEntry entry)
{
    if (entry_no < 0 || entry_no > log_header.n_entries)
        return LOG_OUT_OF_RANGE;
    writeEntry(entry_no, entry);
    return LOG_OK;
}

struct DataLogEntry
{
    byte moisture = 21;
    byte humidity = 22;
    byte brightness = 23;
    byte temperature = 24;
};

LogStatus BaseDataLogger::appendEntry(LogEntry entry)
{
    if (log_header.n_entries + 1 > maxEntries())
        return LOG_FULL;
    log_header.n_entries++;
    writeEntry(log_header.n_entries, entry);

    Serial.println("\n########## DATA_LOG_ENTRY ##########");

    DataLogEntry const *entry_ptr = reinterpret_cast<DataLogEntry const *>(entry);
    Serial.println(entry_ptr->brightness);
    Serial.println(entry_ptr->humidity);
    Serial.println(entry_ptr->moisture);
    Serial.println(entry_ptr->temperature);

    Serial.println("########## DATA_LOG_ENTRY ##########\n");

    writeHead();
    return LOG_OK;
}

unsigned long BaseDataLogger::countEntries()
{
    return log_header.n_entries;
}

unsigned long BaseDataLogger::maxEntries()
{
    //return (log_header.log_size - sizeof(LogHeader)) / log_header.entry_size;
    return log_header.log_size;
}

void BaseDataLogger::clearEntries()
{
    readHead();
    createLog(log_header_index, log_header.log_size, log_header.entry_size);
}
