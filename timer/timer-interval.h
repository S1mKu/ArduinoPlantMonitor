#ifndef _TIMER_INTERVAL_H_
#define _TIMER_INTERVAL_H_

struct TimeoutInterval
{
    uint8_t hours = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
    bool match_hours = false;
    bool match_minutes = false;
    bool match_seconds = false;
};

#endif