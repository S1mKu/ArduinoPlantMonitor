#ifndef _RTC_H_
#define _RTC_H_

#include "ds3231.h"

typedef struct ts Time;

class Timer
{
    public:
        void begin();
        void get_time(Time *t);
        void set_time(Time t);

        void set_alarm();
        void set_alarm_intervall();
}

extern Timer TIMER;

#endif