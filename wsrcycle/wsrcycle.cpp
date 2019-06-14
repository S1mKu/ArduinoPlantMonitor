#include "wsrcycle.h"

#define BUFF_MAX 256

#define LEAP_YEAR(Y) ((Y > 0) && !(Y % 4) && ((Y % 100) || !(Y % 400)))

WSRCycle WSR;

int _skipped_cycles;

unsigned int _timer_interrupt_pin;
unsigned int _ble_interrupt_pin;

tmElements_t _alarm_time;
ALARM_TYPES_t _alarm_type;

WSRCycleDuration _cycle_duration;

volatile bool _timer_flag = true;
volatile bool _ble_flag = false;
volatile int _timer_interrupt_counter = 0;

static const uint8_t _month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

unsigned long start = 0;

void _print_time(tmElements_t &time)
{
    char buff[BUFF_MAX];
    snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d",
             time.Year,
             time.Month,
             time.Day,
             time.Hour,
             time.Minute,
             time.Second);
    Serial.println(buff);
}

int WSRCycle::begin(
    WSRCycleDuration cycle_duration,
    unsigned int timer_interrupt_pin = STD_TIMER_INTERRUPT_PIN,
    unsigned int ble_interrupt_pin = STD_BLE_INTERRUPT_PIN)
{
    _timer_interrupt_pin = timer_interrupt_pin;
    _ble_interrupt_pin = ble_interrupt_pin;
    _cycle_duration = cycle_duration;

    _skipped_cycles = 0;

    Serial.println(_cycle_duration.seconds);
    Serial.println(_cycle_duration.minutes);
    Serial.println(_cycle_duration.hours);

    /*
    Makes sure that all the values are valid.
     */
    if (((_cycle_duration.hours / 24) + (_cycle_duration.minutes / 60) + (_cycle_duration.seconds / 60)) != 0)
    {
        return -1;
    }

    /*
    At least one of the values in 'WSRCycleDuration duration' 
    has to be a number other than 0.
     */
    if (_cycle_duration.hours)
    {
        _alarm_type = ALM1_MATCH_HOURS;
    }
    else if (_cycle_duration.minutes)
    {
        _alarm_type = ALM1_MATCH_MINUTES;
    }
    else if (_cycle_duration.seconds)
    {
        if (_cycle_duration.seconds == 1)
        {
            _alarm_type = ALM1_EVERY_SECOND;
        }
        else
        {
            _alarm_type = ALM1_MATCH_SECONDS;
        }
    }
    else
    {
        return -2;
    }

    pinMode(_timer_interrupt_pin, INPUT);

    // initializes i2c for us
    DS3232RTC::begin();

    // get current time of the RTC and init _alarm_time
    DS3232RTC::read(_alarm_time);

    _print_time(_alarm_time);

    /*
    Prerequisite to allow timed interrupts later on, sets 
    INTCN (bit 3) of the control register to 1.
     */
    squareWave(SQWAVE_NONE);

    /*
    Initialize the alarms to known valuesand clear the alarm flags.
     */
    setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    alarm(ALARM_1);
    alarm(ALARM_2);

    /*
    Set A1IE (bit 1) and A2IE (bit 2) of the control register 
    in order to enable/disable timer interrupts.

    Requieres INTCN (bit 3) of the control register to be set to 1.
     */
    alarmInterrupt(ALARM_1, true);
    alarmInterrupt(ALARM_2, false);

    // clear the alarm flag once again
    alarm(ALARM_1);

    Serial.println(readRTC(0xe));
    Serial.println(readRTC(0xf));
    delay(200);

    return 0; // returns 0 on success
}

int WSRCycle::skipped_cycles()
{
    return _skipped_cycles;
}

void TIMER_ISR(void)
{
    detachInterrupt(digitalPinToInterrupt(_timer_interrupt_pin));
    _timer_flag = true;

    Serial.println("TIMER_ISR 2");
}

bool _a_before_b(tmElements_t time_a, tmElements_t time_b)
{
    if (time_a.Year < time_b.Year)
    {
        return true;
    }
    else if (time_a.Year == time_b.Year)
    {
        if (time_a.Month < time_b.Month)
        {
            return true;
        }
        else if (time_a.Month == time_b.Month)
        {
            if (time_a.Day < time_b.Day)
            {
                return true;
            }
            else if (time_a.Day == time_b.Day)
            {
                if (time_a.Hour < time_b.Hour)
                {
                    return true;
                }
                else if (time_a.Hour == time_b.Hour)
                {
                    if (time_a.Minute < time_b.Minute)
                    {
                        return true;
                    }
                    else if (time_a.Minute == time_b.Minute)
                    {
                        if (time_a.Second < time_b.Second)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

void WSRCycle::_set_next_alarm(void)
{
    tmElements_t cur_time;

    Serial.println("\n----------------------------");
    // display alarm time
    Serial.print("Current alarm time: ");
    _print_time(_alarm_time);

    DS3232RTC::read(cur_time);

    // display current time
    Serial.print("Current time: \t\t");
    _print_time(cur_time);
    Serial.println("");

    while (_a_before_b(_alarm_time, cur_time))
    {
        Serial.println("_alarm_time before cur_time");

        if (_timer_flag)
        {
            _timer_flag = false;
            _skipped_cycles = 0;
        }
        else
        {
            _skipped_cycles++;
        }

        _alarm_time.Second += _cycle_duration.seconds;
        _alarm_time.Minute += _alarm_time.Second / 60;
        _alarm_time.Second %= 60;

        _alarm_time.Minute += _cycle_duration.minutes;
        _alarm_time.Hour += _alarm_time.Minute / 60;
        _alarm_time.Minute %= 60;

        _alarm_time.Hour += _cycle_duration.hours;
        _alarm_time.Day += _alarm_time.Hour / 24;
        _alarm_time.Hour %= 24;

        uint8_t leap_year_day = LEAP_YEAR(_alarm_time.Year) && _alarm_time.Month == 2 ? 1 : 0;
        uint8_t month_index = _alarm_time.Month - 1;
        _alarm_time.Month += _alarm_time.Day / (_month_days[month_index] + leap_year_day);
        _alarm_time.Day %= _month_days[month_index] + leap_year_day;

        _alarm_time.Year += _alarm_time.Month / 12;
        _alarm_time.Month %= 12;

        // display alarm time
        Serial.print("Current alarm time: ");
        _print_time(_alarm_time);

        DS3232RTC::read(cur_time);

        Serial.print("Current time: \t\t");
        _print_time(cur_time);
    }

    setAlarm(_alarm_type, _alarm_time.Second, _alarm_time.Minute, _alarm_time.Hour, 0);

    Serial.println("_alarm_time after cur_time");

    // display alarm time
    Serial.print("New alarm time: ");
    _print_time(_alarm_time);

    Serial.print("Skipped cycles: ");
    Serial.println(_skipped_cycles);

    Serial.println("----------------------------\n");
    delay(200);
}

tmElements_t WSRCycle::get_wakeup_time()
{
    return _alarm_time;
}

void WSRCycle::_shutdown(void)
{
    Serial.println("... shutdown");
    Serial.println(millis() - start);
    delay(100);

    /*
    The interrupt ISR, recoginizing RTC alarms, 
    detaches/disables itself during its first invokation 
    and has to be reattached again in order to recognize 
    the next RTC alarm. This behaviour is not enforced 
    by the Arduino, but intentionally established in 
    'TIMER_ISR(void) {...}'.
    However, this means that once the Arduino invariably 
    enters sleep mode, it is of utter importance that the 
    interrupt ISR for recognizing RTC alarms is in place. 
    Otherwise the Arduino will remain in sleep mode. 
    Prohibiting the execution of any ISR between checking 
    whether an interrupt by the RTC already got caught, 
    causing the ISR to detach/disables itself, and finally 
    entering sleep mode prevents us from running into this 
    issue.

    'cli()' disables interrupts, delaying the execution of any ISR
    'sei()' enables interrupts, allowing the execution of any delayed ISR
     */
    cli();
    if (_timer_flag || _ble_flag)
    {
        sei();
        return;
    }

    power_all_disable();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    /* 
    Command after sei() will be executed immediately 
    not allowing any ISR to be invoked in between.
    */
    sei();
    sleep_mode();
    /* Execution will resume here. */
    sleep_disable();
    power_all_enable();

    Serial.println("continuing");
}

int WSRCycle::sleep()
{
    Serial.println("going to sleep now");
    delay(100);

    if (alarm(ALARM_1))
    {
        Serial.println("oooooooo");
        return 1;
    }

    start = millis();

    /* 
    This method returns once it is asserted that 
    "current time <= _alarm_time" holds true.
     */
    _set_next_alarm();

    /*
    It is not assured that in between setting up 
    the RTC alarm and the attachment of the interrupt 
    the condition "current time <= _alarm_time" holds 
    true anymore. Consequently, the RTC might have 
    exceeded _alarm_time with no interrupt ISR on the 
    Arduion set up to recognize the alarm. 
    This might cause the program to enter sleep mode 
    while not waking up for many cycles. 

    The next if clause protects us against this scenario. 
     */
    attachInterrupt(digitalPinToInterrupt(_timer_interrupt_pin), TIMER_ISR, LOW);

    tmElements_t cur_time;
    DS3232RTC::read(cur_time);

    /*
    Assures that the program only enters sleep mode when 
    the interrupt ISR on the Arduino, recognizing the 
    alarms generated by the RTC, got set up before the 
    RTC time equals or exceeded _alarm_time. 

    This protects us against having no proper RTC alarm 
    timing and ISR in place to wake up the Arduino.
     */
    if (!_a_before_b(cur_time, _alarm_time))
    {
        Serial.println("cur_time not before _alarm_time");

        detachInterrupt(digitalPinToInterrupt(_timer_interrupt_pin));
        _timer_flag = true;
        alarm(ALARM_1);
        return 1;
    }

    /* make Arduino enter sleep mode */
    _shutdown();

    if (alarm(ALARM_1))
    {
        return 1;
    }

    return 0;
}
