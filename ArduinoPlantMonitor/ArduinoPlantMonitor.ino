#include "wsrcycle.h"
#include "SensorDataLog.h"
#include "lcdfeedback.h"
#include "mooddisplay.h"
#include "DHT.h"

#define WSR_TIMER_INTERRUPT_PIN 2
#define WSR_BLE_INTERRUPT_PIN 19
#define WSR_LCD_INTERRUPT_PIN 3

LCD_Feedback lcd(42, 41, 40, 39, 38, 37, 46);
MoodDisplay mood(24, 23, 22);
DHT dht(7, DHT11);

struct WSRCycleDuration wsr_cycle;

tmElements_t time;

struct SensorDataLogEntry
{
    int moisture = 0;
    int brightness = 0;
    float humidity = 0;
    float temperature = 0;
    tmElements_t log_time;
} sensor_data;

void setup()
{
    Serial.begin(9600);

    Serial.println("\n\n########## SETUP ##########\n");

    dht.begin();

    mood.setup();

    Serial.println(sizeof(SensorDataLogEntry));
    Serial.println(sizeof(sensor_data));
    Serial.println(sizeof(tmElements_t));
    Serial.println(sizeof(sensor_data.log_time));

    LOG.begin(sizeof(SensorDataLogEntry));
    LOG.select_last(LOG_ENTRY sensor_data);

    print_time(sensor_data.log_time);

    pinMode(A1, INPUT);
    pinMode(A2, INPUT);

    wsr_cycle.hours = 0;
    wsr_cycle.minutes = 0;
    wsr_cycle.seconds = 30;

    if (WSR.begin(
            wsr_cycle,
            &sensor_data.log_time,
            WSR_TIMER_INTERRUPT_PIN,
            WSR_BLE_INTERRUPT_PIN,
            WSR_LCD_INTERRUPT_PIN) != 0)
    {
        while (true)
        {
            Serial.println("WSRCycle initialization error");
            delay(2000);
        }
    }

    Serial.println("\n\n########## SETUP FINISHED ##########\n");
}

void loop()
{
    Serial.println("looping ...");

    int wakeup_cause = WSR.sleep();

    if (wakeup_cause == 1)
    {
        read_n_store_sensor_data();
    }
    else if (wakeup_cause == 2)
    {
        lcd.set_feedback(sensor_data.temperature, sensor_data.humidity, sensor_data.moisture, sensor_data.brightness);
        lcd.on();
        Serial.println("##### LCD #####");
    }
    else if (wakeup_cause == 3)
    {
        lcd.off();
        Serial.println("##### WDT #####");
    }
    else if (wakeup_cause == 4)
    {
        Serial.println("##### BLE #####");
    }
    else
    {
        Serial.println("##### --- #####");
    }
}

void read_n_store_sensor_data()
{
    /*
    Read the sensor data.
     */
    int moisture = analogRead(A2);
    int brightness = analogRead(A1);
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    WSR.get_cur_time(time);

    /*
    Store the previously read sensor data.
     */
    sensor_data.brightness = brightness;
    sensor_data.humidity = hum;
    sensor_data.temperature = temp;
    sensor_data.log_time = time;

    print_time(sensor_data.log_time);

    Serial.print("Brightness: ");
    Serial.println(sensor_data.brightness);
    Serial.print("Humidity: ");
    Serial.println(sensor_data.humidity);
    Serial.print("Temperature: ");
    Serial.println(sensor_data.temperature);
    Serial.print("Moisture: ");
    Serial.println(sensor_data.moisture);

    LOG.log(LOG_ENTRY sensor_data);

    if (moisture <= 100)
    {
        mood.set_sad_face();
    }
    else
    {
        mood.set_smile();
    }

    struct SensorDataLogEntry entry;

    LOG.select_last(LOG_ENTRY entry);

    Serial.println("\n----------------- SensorDataLogEntry -----------------");
    print_time(entry.log_time);

    Serial.print("Brightness: ");
    Serial.println(entry.brightness);
    Serial.print("Humidity: ");
    Serial.println(entry.humidity);
    Serial.print("Temperature: ");
    Serial.println(entry.temperature);
    Serial.print("Moisture: ");
    Serial.println(entry.moisture);

    Serial.println("----------------- SensorDataLogEntry -----------------\n");
}

void print_time(tmElements_t &time)
{
    char buff[256];
    snprintf(buff, 256, "%d.%02d.%02d %02d:%02d:%02d",
             time.Year,
             time.Month,
             time.Day,
             time.Hour,
             time.Minute,
             time.Second);
    Serial.println(buff);
}
