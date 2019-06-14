#include "timer.h"

TimeoutInterval interval;

void setup()
{
    Serial.begin(9600);

    Serial.println("\n\n\n\n##### starting setup #####\n");
    delay(100);

    interval.seconds = 5;
    interval.minutes = 0;
    interval.hours = 0;
    interval.match_seconds = 1;
    interval.match_minutes = 0;
    interval.match_hours = 0;

    TIMER.set_interval(interval);
    TIMER.begin();

    // Wire.begin();
    // DS3231_init(DS3231_CONTROL_INTCN);
    // DS3231_clear_a2f();

    // // Serial.println(" mh mh mh? klapts jetzt oder nicht, mh<?o ohhh, ein Eis !!!");
    // // delay(100);

    // // pinMode(2, INPUT);
    // // attachInterrupt(digitalPinToInterrupt(2), EXT_ISR2, RISING);

    // // Serial.println("oder WAS?!!\"!!\"");
    // // delay(100);

    // set_next_alarm();

    Serial.println("\n##### finished setup #####\n");
    delay(100);
}

void loop()
{
    // char buff[BUFF_MAX];
    // unsigned long now = millis();
    // struct ts t;

    // // once a while show what is going on
    // if ((now - prev > interval) && (Serial.available() <= 0))
    // {
    //     DS3231_get(&t);

    //     // display current time
    //     snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
    //              t.mon, t.mday, t.hour, t.min, t.sec);
    //     Serial.println(buff);

    //     // display a2 debug info
    //     DS3231_get_a2(&buff[0], 59);
    //     Serial.println(buff);

    //     //     if (external_interrupt_flag)
    //     //     {
    //     //         external_interrupt_flag = false;
    //     //         Serial.println("\n########## EXTERNAL INTERRUPT ##########\n");
    //     //         attachInterrupt(digitalPinToInterrupt(2), EXT_ISR2, RISING);
    //     //     }

    //     if (DS3231_triggered_a2())
    //     {
    //         Serial.println(" -> alarm2 has been triggered");
    //         set_next_alarm();
    //         // clear a2 alarm flag and let INT go into hi-z
    //         DS3231_clear_a2f();
    //     }
    //     prev = now;
    // }

    delay(1000);

    if (_alarm_flag)
    {
        _alarm_flag = false;
        TIMER.set_next_alarm();
    }
}
