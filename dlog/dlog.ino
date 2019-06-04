#include "SensorDataLog.h"

void setup()
{
    int err_no = 0;

    Serial.begin(9600);

    Serial.println("\n########## STARTUP ##########\n");
    Serial.println((unsigned long)sizeof(LogHeader));

    delay(200);

    if ((err_no = LOG.begin()) != 0)
    {
        Serial.print("LOG.begin(...) - ERROR ");
        Serial.println(err_no);
    }
    Serial.println(err_no);

    delay(200);

    Serial.println("\nlog first entry\n");

    DATA_LOG_ENTRY.brightness = 5;
    DATA_LOG_ENTRY.humidity = 6;
    DATA_LOG_ENTRY.moisture = 7;
    DATA_LOG_ENTRY.temperature = 8;

    if ((err_no = LOG.log(DATA_LOG_ENTRY)) != 0)
    {
        Serial.print("LOG.log(...) - ERROR ");
        Serial.println(err_no);
    }
    Serial.println(err_no);

    Serial.println("\n\nsetup complete\n\n");

    delay(200);
}

void loop()
{
    DATA_LOG_ENTRY.brightness = 1;
    DATA_LOG_ENTRY.humidity = 2;
    DATA_LOG_ENTRY.moisture = 3;
    DATA_LOG_ENTRY.temperature = 4;

    int err_code = LOG.select_last(&DATA_LOG_ENTRY);
    Serial.println(err_code);

    Serial.println(DATA_LOG_ENTRY.brightness);
    Serial.println(DATA_LOG_ENTRY.humidity);
    Serial.println(DATA_LOG_ENTRY.moisture);
    Serial.println(DATA_LOG_ENTRY.temperature);

    DataLogEntry data_entry;
    data_entry.brightness = 9;
    data_entry.humidity = 8;
    data_entry.moisture = 7;
    data_entry.temperature = 6;

    err_code = LOG.log(data_entry);
    Serial.println(err_code);

    DATA_LOG_ENTRY.brightness = 0;
    DATA_LOG_ENTRY.humidity = 0;
    DATA_LOG_ENTRY.moisture = 0;
    DATA_LOG_ENTRY.temperature = 0;

    err_code = LOG.select_last(&DATA_LOG_ENTRY);
    Serial.println(err_code);

    Serial.println("-------------------------------");

    Serial.println(DATA_LOG_ENTRY.brightness);
    Serial.println(DATA_LOG_ENTRY.humidity);
    Serial.println(DATA_LOG_ENTRY.moisture);
    Serial.println(DATA_LOG_ENTRY.temperature);

    delay(60000);
}
