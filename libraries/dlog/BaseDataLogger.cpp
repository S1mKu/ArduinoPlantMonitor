#include "BaseDataLogger.h"

LogHeader log_header;

uint32_t log_header_index;
uint32_t first_entry_index;

void BaseDataLogger::writeHead()
{
    Serial.println("\n########## write header ##########");
    Serial.print("entry_size ");
    Serial.println(log_header.entry_size);
    Serial.print("flag ");
    Serial.println(log_header.flag);
    Serial.print("log_size ");
    Serial.println(log_header.log_size);
    Serial.print("n_entries ");
    Serial.println(log_header.n_entries);
    Serial.println(log_header_index);

    write(log_header_index, LOG_ENTRY log_header, (uint32_t)sizeof(LogHeader));

    Serial.println("########## write header ##########\n");
}

void BaseDataLogger::readHead()
{
    read(log_header_index, LOG_ENTRY log_header, sizeof(LogHeader));

    Serial.println("\n########## read header ##########");
    Serial.print("sizeof ");
    Serial.println(sizeof(LogHeader));
    Serial.print("entry_size ");
    Serial.println(log_header.entry_size);
    Serial.print("flag ");
    Serial.println(log_header.flag);
    Serial.print("log_size ");
    Serial.println(log_header.log_size);
    Serial.print("n_entries ");
    Serial.println(log_header.n_entries);
    Serial.println(log_header_index);

    Serial.println("########## read header ##########\n");
}

LogStatus BaseDataLogger::createLog(uint32_t header_index, uint32_t tablesize, uint32_t entrysize)
{
    log_header_index = header_index;
    first_entry_index = sizeof(LogHeader) + log_header_index;
    log_header.flag = LOG_FLAG;
    log_header.n_entries = 0;
    log_header.entry_size = entrysize;
    log_header.log_size = tablesize;

    Serial.println("\n########## createLog header ##########");
    Serial.print("entry_size ");
    Serial.println(log_header.entry_size);
    Serial.print("flag ");
    Serial.println(log_header.flag);
    Serial.print("log_size ");
    Serial.println(log_header.log_size);
    Serial.print("n_entries ");
    Serial.println(log_header.n_entries);
    Serial.println(log_header_index);

    Serial.println("########## createLog header ##########\n");

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

LogStatus BaseDataLogger::openLog(uint32_t header_index)
{
    log_header_index = header_index;
    first_entry_index = sizeof(LogHeader) + log_header_index;
    readHead();
    return LOG_OK;
}

LogStatus BaseDataLogger::writeEntry(uint32_t entry_no, const LogEntry entry)
{
    Serial.print("writeEntry: ");
    Serial.println(log_header.entry_size);

    write(first_entry_index + ((entry_no - 1) * log_header.entry_size), entry, log_header.entry_size);
    return LOG_OK;
}

LogStatus BaseDataLogger::readEntry(uint32_t entry_no, LogEntry entry)
{
    if (entry_no < 1 || entry_no > log_header.n_entries)
        return LOG_OUT_OF_RANGE;
    read(first_entry_index + ((entry_no - 1) * log_header.entry_size), entry, log_header.entry_size);
    return LOG_OK;
}

LogStatus BaseDataLogger::deleteEntry(uint32_t entry_no)
{
    if (entry_no < 0 || entry_no > log_header.n_entries)
        return LOG_OUT_OF_RANGE;
    LogEntry entry = (uint8_t *)malloc(log_header.entry_size);
    for (uint32_t i = entry_no + 1; i <= log_header.n_entries; i++)
    {
        readEntry(i, entry);
        writeEntry(i - 1, entry);
    }
    free(entry);
    log_header.n_entries--;
    writeHead();
    return LOG_OK;
}

LogStatus BaseDataLogger::insertEntry(uint32_t entry_no, LogEntry entry)
{
    if (countEntries() == maxEntries())
        return LOG_FULL;
    if (countEntries() > 0 && (entry_no < 0 || entry_no > log_header.n_entries))
        return LOG_OUT_OF_RANGE;
    if (countEntries() == 0 && entry_no == 1)
        return appendEntry(entry);

    LogEntry buf = (uint8_t *)malloc(log_header.entry_size);
    for (uint32_t i = log_header.n_entries; i >= entry_no; i--)
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

LogStatus BaseDataLogger::updateEntry(uint32_t entry_no, LogEntry entry)
{
    if (entry_no < 0 || entry_no > log_header.n_entries)
        return LOG_OUT_OF_RANGE;
    writeEntry(entry_no, entry);
    return LOG_OK;
}

LogStatus BaseDataLogger::appendEntry(LogEntry entry)
{
    Serial.println("appendEntry");

    if (log_header.n_entries + 1 > maxEntries())
        return LOG_FULL;
    log_header.n_entries++;
    writeEntry(log_header.n_entries, entry);
    writeHead();
    return LOG_OK;
}

uint32_t BaseDataLogger::countEntries()
{
    return log_header.n_entries;
}

uint32_t BaseDataLogger::maxEntries()
{
    return log_header.log_size;
}

void BaseDataLogger::clearEntries()
{
    readHead();
    createLog(log_header_index, log_header.log_size, log_header.entry_size);
}
