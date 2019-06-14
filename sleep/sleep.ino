#include "sleep.h"

TimeoutInterval timeout_interval;

unsigned long i = 1;
unsigned long ii = 0;

unsigned long start;
unsigned long end;

unsigned long time_passed = 0;

void setup(void)
{
    Serial.begin(9600);

    Serial.println("\n\n\n########## STARTING ##########\n");

    timeout_interval.seconds = 10;
    timeout_interval.minutes = 0;
    timeout_interval.hours = 0;
    timeout_interval.match_seconds = 1;
    timeout_interval.match_minutes = 0;
    timeout_interval.match_hours = 0;

    configure_sleep_mode(timeout_interval);
    setup_sleep_interrupts();

    start = millis();

    // Serial.println("............................................................");
    // Serial.println(".........1.........2.........3.........4.........5.........6");
}

void loop(void)
{
    // Serial.println("\n---------------------------\n");
    // Serial.println("going to sleep");

    //delay(100);
    // if (i % 60 == 0)
    // {
    //     i = 1;
    //     Serial.println("");
    // }
    // i++;

    // int interrupt_cause = enter_sleep_mode();

    // Serial.println("woke up again");

    // if (interrupt_cause == 0)
    //     Serial.println("BLE INTERRUPT");
    // else if (interrupt_cause == 1)
    //     Serial.println("TIMER INTERRUPT");
    // else
    //     Serial.println("UNKNOWN INTERRUPT CAUSE");

    // time_passed = millis() - start;

    // //Serial.println(time_passed);
    // //Serial.println(start);

    // if (time_passed >= i * 1000)
    // {
    //     enter_sleep_mode();

    //     //Serial.println(time_passed);

    //     ii = time_passed / 1000;

    //     for (i; i < ii; i++)
    //     {
    //         if (i % 10 == 0)
    //         {
    //             if (i % 60 == 0)
    //             {
    //                 Serial.println("6");
    //                 start += time_passed;
    //             }
    //             else
    //             {
    //                 Serial.print((i / 10) % 6);
    //             }
    //         }
    //         else
    //         {
    //             Serial.print(".");
    //         }
    //     }

    //     i %= 60;

    //     // if (i == 0)
    //     //     i = 1;
    // }
    // delay(50);

    // Serial.print(":");
    // Serial.print(i);

    delay(1000);
    enter_sleep_mode();
}
