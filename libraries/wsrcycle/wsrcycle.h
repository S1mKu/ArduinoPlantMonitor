#ifndef _WSRCYCLE_H_
#define _WSRCYCLE_H_

//#include "ds3231-time.h"

#include "TimeLib.h"
#include "DS3232RTC.h"
#include <stdbool.h>
#include <Arduino.h>

#define STD_TIMER_INTERRUPT_PIN 2
#define STD_BLE_INTERRUPT_PIN 19
#define STD_LCD_INTERRUPT_PIN 3

// typedef struct ts Time;

typedef struct WSRCycleDuration
{
    uint8_t hours = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
};

typedef enum WakeupCause
{
    TIMER,
    CALL
};

class WSRCycle : protected DS3232RTC
{
private:
    void _advance_alarm_time(void);
    void _set_next_rtc_alarm(void);
    void _shutdown(void);
    //     volatile WakeupCause _wakeup_cause;

    // int _skipped_cycles;
public:
    WSRCycle(bool initI2C = false) : DS3232RTC(initI2C){};
    int begin(
        WSRCycleDuration duration,
        const tmElements_t *start_time = NULL,
        unsigned int timer_interrupt_pin = STD_TIMER_INTERRUPT_PIN,
        unsigned int ble_interrupt_pin = STD_BLE_INTERRUPT_PIN,
        unsigned int lcd_interrupt_pin = STD_LCD_INTERRUPT_PIN);

    int sleep();
    int skipped_cycles();
    tmElements_t get_wakeup_time();
    int get_cur_time(tmElements_t &time);
};

extern WSRCycle WSR;

#endif