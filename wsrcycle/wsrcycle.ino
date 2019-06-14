#include "wsrcycle.h"

WSRCycleDuration duration;

uint8_t led_pin = 13;

void setup(void)
{
    Serial.begin(9600);

    Serial.println("\n\n\n########## STARTING ##########\n");

    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, LOW);

    duration.seconds = 2;
    duration.minutes = 0;
    duration.hours = 0;

    WSR.begin(duration);

    Serial.println(".................................");
}

void loop(void)
{
    Serial.println("\n\ndo something ...");

    int res = WSR.sleep();

    // // Serial.println(res);
    // // Serial.println(WSR.skipped_cycles());


    digitalWrite(led_pin, HIGH);
    delay(5000);
    digitalWrite(led_pin, LOW);

    // Serial.println(":-)");

    // delay(5000);
}

// #include <avr/sleep.h>

// #define INT_PIN 2
// #define LED_PIN 13

// void INT_PINisr(void)
// /* ISR fuer Pin 2 */
// {
//     /* detach Interrupt, damit er nur einmal auftritt */
//     detachInterrupt(0);
// }

// void enter_sleep(void)
// {
//     attachInterrupt(0, INT_PINisr, LOW);
//     /* Arduino schlafen legen */
//     set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//     sleep_enable();
//     sleep_mode();
//     sleep_disable();
// }

// void setup()
// {
//     Serial.begin(9600);
//     Serial.println("Starte ...");
//     /* Pin 2 als Interrupt-Pin, Pullup einschalten */
//     pinMode(INT_PIN, INPUT);
//     digitalWrite(INT_PIN, LOW);
//     pinMode(LED_PIN, OUTPUT);
//     Serial.println("Init erfolgt ...");
//     delay(100);
// }

// void loop()
// {
//     /* warten, bis der Interrupt-Eingang wieder frei ist */
//     while (digitalRead(INT_PIN) == LOW)
//     {
//         Serial.println("*** Taste loslassen ***");
//         delay(500);
//     }
//     Serial.println("Gehe schlafen ...");
//     delay(100);
//     /* LED umschalten und wieder schlafen */
//     digitalWrite(LED_PIN, !digitalRead(LED_PIN));
//     enter_sleep();
//     Serial.println("Bin aufgewacht ...");
//     delay(100);
// }
