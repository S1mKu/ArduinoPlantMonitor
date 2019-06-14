#ifndef _TIMER_H_
#define _TIMER_H_

#include <inttypes.h>
#include "ds3231-time.h"
#include "timer-interval.h"

#define DEFAULT_INTERRUPT_PIN 2

typedef struct ts Time;

extern volatile bool _alarm_flag;

class Timer
{
public:
    void begin();
    void get_global_time(Time *t);
    void set_global_time(Time t);

    void set_interval(TimeoutInterval i);
    void set_isr(void (*alarm_isr)());

    void set_next_alarm(void);
};

extern Timer TIMER;

#endif