#include "timer.h"
#include "ds3231.h"
#include <Wire.h>

Timer TIMER;

volatile Time _alarm_time;

volatile TimeoutInterval _interval;

volatile void (*_alarm_isr)() = {};

volatile int _interrupt_pin = DEFAULT_INTERRUPT_PIN;

// flags define what calendar component to be checked against the current time in order
// to trigger the alarm
// A2M2 (minutes) (0 to enable, 1 to disable)
// A2M3 (hour)    (0 to enable, 1 to disable)
// A2M4 (day)     (0 to enable, 1 to disable)
// DY/DT          (dayofweek == 1/dayofmonth == 0)

// flags are: A1M1 (seconds), A1M2 (minutes), A1M3 (hour),
// A1M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0)
volatile uint8_t flags[4] = {0, 0, 0, 1};

volatile bool _alarm_flag = false;

void EXT_ISR(void);
void set_next_alarm(void);

#define BUFF_MAX 256

void Timer::begin()
{
    Wire.begin();
    DS3231_init(DS3231_CONTROL_INTCN);

    DS3231_get(&_alarm_time);
    set_next_alarm();

    attachInterrupt(digitalPinToInterrupt(_interrupt_pin), EXT_ISR, HIGH);

    // activate Alarm1
    DS3231_set_creg(DS3231_CONTROL_INTCN | DS3231_CONTROL_A1IE);

    Serial.println(DS3231_get_creg());
    Serial.println(DS3231_get_sreg());

    Serial.println("-----------");

    delay(6000);

    Serial.println("-----------");

    Serial.println(DS3231_get_creg());
    Serial.println(DS3231_get_sreg());
}

void Timer::get_global_time(Time *t)
{
    DS3231_get(t);
}

void Timer::set_global_time(Time t)
{
    DS3231_set(t);
}

void Timer::set_interval(TimeoutInterval i)
{
    _interval = i;
}

void Timer::set_next_alarm(void)
{
    char buff[BUFF_MAX];

    // display current time
    snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d",
             _alarm_time.year,
             _alarm_time.mon,
             _alarm_time.mday,
             _alarm_time.hour,
             _alarm_time.min,
             _alarm_time.sec);
    Serial.println(buff);

    if (_interval.match_hours)
    {
        _alarm_time.hour += _interval.hours;
        _alarm_time.hour %= 24;
        // flags[2] = 0;
    }

    if (_interval.match_minutes)
    {
        _alarm_time.min += _interval.minutes;
        _alarm_time.min %= 60;
        // flags[1] = 0;
    }

    if (_interval.match_seconds)
    {
        _alarm_time.sec += _interval.seconds;
        _alarm_time.sec %= 60;
        // flags[0] = 0;
    }

    // display alarm time
    snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d",
             _alarm_time.year,
             _alarm_time.mon,
             _alarm_time.mday,
             _alarm_time.hour,
             _alarm_time.min,
             _alarm_time.sec);
    Serial.println(buff);

    DS3231_clear_a1f();

    // flags are: A1M1 (seconds), A1M2 (minutes), A1M3 (hour),
    // A1M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0)
    DS3231_set_a1(
        _alarm_time.sec,
        _alarm_time.min,
        _alarm_time.hour,
        0,
        flags);
}

void EXT_ISR(void)
{
    //set_next_alarm();
    //_alarm_isr();
    //detachInterrupt(digitalPinToInterrupt(2));
    Serial.println("huhu");
    // delay(100);

    Serial.println("###");

    // if (_interval.match_hours)
    // {
    //     Serial.println("........ 1");
    //     // delay(100);

    //     _alarm_time.hour += _interval.hours;
    //     _alarm_time.hour %= 24;
    //     // flags[2] = 0;
    // }

    // Serial.println("........");
    // // delay(100);

    // if (_interval.match_minutes)
    // {
    //     Serial.println("........ 2");
    //     // delay(100);

    //     _alarm_time.min += _interval.minutes;
    //     _alarm_time.min %= 60;
    //     // flags[1] = 0;
    // }

    // Serial.println("........");
    // // delay(100);

    // if (_interval.match_seconds)
    // {
    //     Serial.println("........ 3");
    //     // delay(100);

    //     _alarm_time.sec += _interval.seconds;
    //     _alarm_time.sec %= 60;
    //     // flags[0] = 0;
    // }

    // // Serial.println("11111111111111111111111");

    // DS3231_clear_a1f();

    // Serial.println("22222222222222222222222");

    // // flags are: A1M1 (seconds), A1M2 (minutes), A1M3 (hour),
    // // A1M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0)
    // DS3231_set_a1(
    //     _alarm_time.sec,
    //     _alarm_time.min,
    //     _alarm_time.hour,
    //     0,
    //     flags);

    _alarm_flag = true;

    Serial.println("333333333333333333333333333333");
}

void set_isr(void (*alarm_isr)())
{
    _alarm_isr = alarm_isr;
}

void set_interrupt_pin(int pin)
{
    _interrupt_pin = pin;
}
