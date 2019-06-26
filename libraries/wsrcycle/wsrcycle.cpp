#include "wsrcycle.h"
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
// #include <avr/io.h>

#define BUFF_MAX 256

#define LEAP_YEAR(Y) ((Y > 0) && !(Y % 4) && ((Y % 100) || !(Y % 400)))

WSRCycle WSR;

int _skipped_cycles;

volatile unsigned int _timer_interrupt_pin;
volatile unsigned int _ble_interrupt_pin;
volatile unsigned int _lcd_interrupt_pin;

volatile byte _wdtcsr;

tmElements_t _alarm_time;
tmElements_t _timestamp;

ALARM_TYPES_t _alarm_type;

WSRCycleDuration _cycle_duration;

volatile bool _timer_flag = true;
volatile bool _ble_flag = false;
volatile bool _wdt_flag = false;
volatile bool _lcd_flag = false;

volatile int _lcd_interrupt_counter = 0;
volatile int _timer_interrupt_counter = 0;
volatile int _wdt_interrupt_counter = 0;

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

void _watchdog_setup()
{

    //WDP3 - WDP2 - WPD1 - WDP0 - time
    // 0      0      0      0      16 ms
    // 0      0      0      1      32 ms
    // 0      0      1      0      64 ms
    // 0      0      1      1      0.125 s
    // 0      1      0      0      0.25 s
    // 0      1      0      1      0.5 s
    // 0      1      1      0      1.0 s
    // 0      1      1      1      2.0 s
    // 1      0      0      0      4.0 s
    // 1      0      0      1      8.0 s

    /*

    WDTCSR - Watchdog Timer Control Status register

      Bit      7    6    5    4    3    2    1    0
    Flagname  WDIF WDIE WDP3 WDCE WDE  WDP2 WDP1 WDP0


    WDE WDIE         Watchdog Mode                 Action
     0   0               stop                       none
     0   1          interrupt mode                interrupt
     1   0         system reset mode                reset
     1   1    interrupt & system reset mode    interrupt & reset

     */

    /*
    A 'timed' sequence is required for configuring the WDT,
    so we need to disable interrupts here.
     */
    cli();
    wdt_reset();

    /*
    WDE is overridden by WDRF in MCUSR. This means
    that WDE is always set when WDRF is set. To clear
    WDE, WDRF must be cleared first. This feature
    ensures multiple resets during conditions causing
    failure, and a safestart-up after the failure.
     */
    MCUSR &= ~(1 << WDRF);

    /*
    Watchdog Change Enable (WDCE) for changing WDE
    and prescaler bits.
    Once written to one, hardware will clear WDCE
    after four clock cycles. All changes to the 
    prescaler bits have to be finished until then.
     */
    WDTCSR = (1 << WDCE) | (1 << WDE);

    /*
    Set new watchdog timeout value to 8 seconds
    in the WDT control register (WDTCSR)
     */
    WDTCSR = (1 << WDP3) | (1 << WDP0);

    /*
    Enable interrupts instead of system reset.
     */
    WDTCSR &= ~(1 << WDE);
    WDTCSR |= 1 << WDIE;

    sei();

    _wdtcsr = WDTCSR;
}

void TIMER_ISR(void)
{
    detachInterrupt(digitalPinToInterrupt(_timer_interrupt_pin));
    _timer_flag = true;
    _timer_interrupt_counter++;
}

void BLE_ISR(void)
{
    detachInterrupt(digitalPinToInterrupt(_ble_interrupt_pin));
    _ble_flag = true;
}

void LCD_ISR(void)
{
    detachInterrupt(digitalPinToInterrupt(_lcd_interrupt_pin));
    _watchdog_setup();
    _lcd_flag = true;
    _lcd_interrupt_counter++;
}

ISR(WDT_vect)
{
    wdt_reset();
    wdt_disable();
    _wdt_flag = true;
    attachInterrupt(digitalPinToInterrupt(_lcd_interrupt_pin), LCD_ISR, LOW);
    _wdt_interrupt_counter++;
}

int WSRCycle::begin(
    WSRCycleDuration cycle_duration,
    const tmElements_t *start_time = NULL,
    unsigned int timer_interrupt_pin = STD_TIMER_INTERRUPT_PIN,
    unsigned int ble_interrupt_pin = STD_BLE_INTERRUPT_PIN,
    unsigned int lcd_interrupt_pin = STD_LCD_INTERRUPT_PIN)
{
    _skipped_cycles = 0;

    _cycle_duration = cycle_duration;

    Serial.println(WDIF);
    Serial.println(WDIE);
    Serial.println(WDP3);
    Serial.println(WDCE);
    Serial.println(WDE);
    Serial.println(WDP2);
    Serial.println(WDP1);
    Serial.println(WDP0);

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

    _timer_interrupt_pin = timer_interrupt_pin;
    _ble_interrupt_pin = ble_interrupt_pin;
    _lcd_interrupt_pin = lcd_interrupt_pin;

    /*
    Set up pin modes for the interrupt pins.
     */
    pinMode(_timer_interrupt_pin, INPUT_PULLUP);
    pinMode(_ble_interrupt_pin, INPUT_PULLUP);
    pinMode(_lcd_interrupt_pin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(_ble_interrupt_pin), BLE_ISR, LOW);
    attachInterrupt(digitalPinToInterrupt(_lcd_interrupt_pin), LCD_ISR, LOW);

    // initializes i2c for us
    DS3232RTC::begin();

    if (start_time)
    {
        _alarm_time = *start_time;

        Serial.println("Use start_time.");
    }
    else
    {
        // get current time of the RTC and init _alarm_time
        DS3232RTC::read(_alarm_time);

        Serial.println("Use time from RTC.");
    }

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

void WSRCycle::_advance_alarm_time(void)
{
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
}

void WSRCycle::_set_next_rtc_alarm(void)
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

        _advance_alarm_time();

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

int WSRCycle::get_cur_time(tmElements_t &time)
{
    return DS3232RTC::read(time);
}

void WSRCycle::_shutdown(void)
{
    Serial.print("... shutdown ");
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
    if (_timer_flag || _ble_flag || _wdt_flag || _lcd_flag)
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

    Serial.println("woke up again and continue");
}

int WSRCycle::sleep()
{
    start = millis();

    /*
    ------T-----|A     shutdown

    -------|A----T     set next alarm time based on the current time and return
     */

    Serial.println("going to sleep now");
    delay(100);

    if (_lcd_flag)
    {
        _lcd_flag = false;

        Serial.print("LCD ##### ");
        Serial.println(_lcd_interrupt_counter);

        // _watchdog_setup();

        Serial.print("###################### WDT ");
        Serial.println(WDTCSR);
        Serial.print("###################### WDT ");
        Serial.println(_wdtcsr);

        return 2;
    }
    else if (_wdt_flag)
    {
        _wdt_flag = false;

        Serial.print("WDT ##### ");
        Serial.println(_wdt_interrupt_counter);

        Serial.print("###################### WDT ");
        Serial.println(WDTCSR);
        Serial.print("###################### WDT ");
        Serial.println(_wdtcsr);

        // attachInterrupt(digitalPinToInterrupt(_lcd_interrupt_pin), LCD_ISR, LOW);

        return 3;
    }
    else if (_ble_flag)
    {
        _ble_flag = false;
        return 4;
    }

    DS3232RTC::read(_timestamp);

    /*
    It is worth mentioning that even though alarm(ALARM_1) 
    resets the alarm flag of the RTC the flag will be set 
    again immediately when cur_time == _alarm_time. 

    Thus, alarm(ALARM_1) might be a relic of a deprecated
     - because already handled - alarm and uncommenting 
     alarm(ALARM_1) might lead to unwanted program behaviour. 
     */
    if (!_a_before_b(_timestamp, _alarm_time) /* || alarm(ALARM_1) */)
    {
        Serial.println("alarm time passed, returning immediately");
        Serial.println(!_a_before_b(_timestamp, _alarm_time));
        Serial.println(_a_before_b(_timestamp, _alarm_time));
        _print_time(_timestamp);
        delay(100);
        _print_time(_alarm_time);

        _alarm_time = _timestamp;

        /* 
        This method increments _alarm_time by the 
        time durations specified in _cycle_duration. 
        Due to the previous line of code this assures 
        that "current time <= _alarm_time" holds true.
        */
        _advance_alarm_time();

        // _print_time(_timestamp);
        _print_time(_alarm_time);

        Serial.print("TIMER ##### ");
        Serial.println(_timer_interrupt_counter);

        _timer_flag = false;
        return 1;
    }

    /* 
    This method returns once it is asserted that 
    "current time <= _alarm_time" holds true.
     */
    // _set_next_rtc_alarm();

    /*
    Since !_a_before_b(_timestamp, _alarm_time) || alarm(ALARM_1)
    got evaluated to false '_timestamp < _alarm_time' should hold 
    true for now.

    Reset the alarm flag of the RTC with alarm(ALARM_1) and set a 
    new alarm time.
     */
    alarm(ALARM_1);
    setAlarm(_alarm_type, _alarm_time.Second, _alarm_time.Minute, _alarm_time.Hour, 0);

    /*
    It is not assured that in between setting up 
    the RTC alarm and the attachment of the interrupt 
    the condition "current time < _alarm_time" or even 
    "current time <= _alarm_time" holds true anymore. 
    Consequently, the RTC might have exceeded 
    _alarm_time with no interrupt ISR on the Arduion 
    set up to recognize the alarm. 
    This might cause the program to enter sleep mode 
    while not waking up for many cycles. 

    The next if clause protects us against this scenario. 
     */
    attachInterrupt(digitalPinToInterrupt(_timer_interrupt_pin), TIMER_ISR, LOW);

    DS3232RTC::read(_timestamp);

    /*
    Assures that the program only enters sleep mode when 
    the interrupt ISR on the Arduino, recognizing the 
    alarms generated by the RTC, got set up before the 
    RTC time exceeded _alarm_time. 

    This protects us against having no proper RTC alarm 
    timing and ISR in place to wake up the Arduino.
     */
    if (!_a_before_b(_timestamp, _alarm_time) || alarm(ALARM_1))
    {
        Serial.println("cur_time not before _alarm_time");

        /*
        Since we are not entering sleep mode we need no interrupt 
        to wake the Arduino up again.
         */
        detachInterrupt(digitalPinToInterrupt(_timer_interrupt_pin));

        /*
        We exceeded _alarm time and have to set a new one now.
         */
        _advance_alarm_time();

        Serial.print("TIMER ");
        Serial.println(_timer_interrupt_counter);

        _timer_flag = false;
        return 1;
    }

    /* make Arduino enter sleep mode */
    _shutdown();

    /*
    It is worth mentioning that even though alarm(ALARM_1) 
    resets the alarm flag of the RTC the flag will be set 
    again immediately when cur_time == _alarm_time.
     */
    if (_timer_flag || alarm(ALARM_1))
    {
        /*
        The RTC alarm got fired so we exceeded _alarm time 
        and have to set a new one now.
         */
        _advance_alarm_time();

        Serial.print("##### TIMER ");
        Serial.println(_timer_interrupt_counter);

        _timer_flag = false;
        return 1;
    }
    else if (_lcd_flag)
    {
        _lcd_flag = false;

        Serial.print("##### LCD ");
        Serial.println(_lcd_interrupt_counter);

        // _watchdog_setup();

        Serial.print("###################### WDT ");
        Serial.println(WDTCSR);
        Serial.print("###################### WDT ");
        Serial.println(_wdtcsr);

        return 2;
    }
    else if (_wdt_flag)
    {
        _wdt_flag = false;

        Serial.print("##### WDT ");
        Serial.println(_wdt_interrupt_counter);

        Serial.print("###################### WDT ");
        Serial.println(WDTCSR);
        Serial.print("###################### WDT ");
        Serial.println(_wdtcsr);

        // attachInterrupt(digitalPinToInterrupt(_lcd_interrupt_pin), LCD_ISR, LOW);

        return 3;
    }
    else if (_ble_flag)
    {
        _ble_flag = false;
        return 4;
    }

    return 0;
}
