#ifndef SLEEP_H
#define SLEEP_H

#include <stdbool.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "timer-interval.h"

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define STD_BLE_INTERRUPT_PIN (3)
#define STD_TIMER_INTERRUPT_PIN (2)

void configure_sleep_mode(
    TimeoutInterval interval,
    unsigned int timer_interrupt_pin = STD_TIMER_INTERRUPT_PIN,
    unsigned int ble_interrupt_pin = STD_BLE_INTERRUPT_PIN);

/**
   returns 0 for an external interrupt and 1 for a wdt interrupt and -1 otherwise
*/
int enter_sleep_mode(void);

void setup_sleep_interrupts(void);

#endif
