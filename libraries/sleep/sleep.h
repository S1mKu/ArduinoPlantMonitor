#ifndef SLEEP_H
#define SLEEP_H

#include <stdbool.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define STD_INTERRUPT_PIN (2)

void configure_sleep_mode(unsigned int interrupt_pin, unsigned int sleep_duration = STD_INTERRUPT_PIN);

/**
   returns 0 for an external interrupt and 1 for a wdt interrupt and -1 otherwise
*/
int enter_sleep_mode(void);


#endif
