#include "sleep.h"

int interrupt_pin = STD_INTERRUPT_PIN;

/**
  @brief   Flag stating if sleep has been entered or not.
*/
volatile bool sleep_entered = false;
volatile bool external_interrupt_flag = false;
volatile bool wdt_interrupt_flag = false;

volatile int wdt_interrupt_counter = 0;
volatile int wdt_interrupt_limit = 8;

void EXT_ISR(void)
{
  detachInterrupt(digitalPinToInterrupt(interrupt_pin));
  external_interrupt_flag = true;
}

/**
  @brief   Register the watchdog interrupt service routine.
*/
ISR(WDT_vect)
{
  wdt_interrupt_counter++;
  wdt_interrupt_counter = wdt_interrupt_counter % wdt_interrupt_limit;

  if (wdt_interrupt_counter == 0)
  {
    wdt_interrupt_flag = true;
  }
}

/**
   @brief We need to enable the WDT for Interrupt mode only, so we can use it
          as a sleep wakeup timer as well as for detecting lockups.
   @note  Inline function!
*/
inline void configure_wdt(void)
{
  /* A 'timed' sequence is required for configuring the WDT, so we need to
     disable interrupts here.
  */
  cli();
  wdt_reset();

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

  MCUSR &= ~_BV(WDRF);
  /* Start the WDT Config change sequence. */
  WDTCSR |= _BV(WDCE) | _BV(WDE);
  /* Configure the prescaler and the WDT for interrupt mode only*/
  //WDTCSR =  _BV(WDP1) | _BV(WDP2) | _BV(WDIE);
  WDTCSR =  _BV(WDP0) | _BV(WDP3) | _BV(WDIE);

  wdt_reset();
  sei();
}

void configure_sleep_mode(unsigned int sleep_duration, unsigned int interrupt_pin = STD_INTERRUPT_PIN)
{
  /*
     setup for the external interrupt pin
  */
  interrupt_pin = interrupt_pin;
  pinMode(interrupt_pin, INPUT_PULLUP);

  /*
     setup for the wdt (NOT YET IMPLEMENTED)

     TODO: implement wdt setup
  */
}

/**
   returns 0 for an external interrupt and 1 for a wdt interrupt and -1 otherwise
*/
int enter_sleep_mode(void)
{
  /* reset flags and counters */
  {
    cli();

    if (wdt_interrupt_flag)
    {
      wdt_interrupt_counter = 0;
      wdt_interrupt_flag = false;
    }

    if (external_interrupt_flag)
    {
      external_interrupt_flag = false;
    }

    sei();
  }

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  power_adc_disable();      // ADC converter
  power_spi_disable();      // SPI
  power_usart0_disable();   // Serial 0 (USART)
  power_usart1_disable();   // Serial 1 (USART)
  power_usart2_disable();   // Serial 2 (USART)
  power_usart3_disable();   // Serial 3 (USART)
  //power_timer0_disable();   // Timer 0
  power_timer1_disable();   // Timer 1
  power_timer2_disable();   // Timer 2
  power_twi_disable();      // TWI (I2C)

  cli();

  if (external_interrupt_flag || wdt_interrupt_flag)
  {
    sei();
    return;
  }

  /* Setup INT_PIN as an interrupt and attach handler. */
  attachInterrupt(digitalPinToInterrupt(interrupt_pin), EXT_ISR, LOW);

  sleep_entered = true;
  sleep_enable();

  // turn off brown-out enable in software
  //  MCUCR = bit (BODS) | bit (BODSE);
  //  MCUCR = bit (BODS);

  sei();

  sleep_mode();
  /* Execution will resume here. */
  sleep_disable();

  //  power_adc_enable();        // ADC converter
  //  power_spi_enable();        // SPI
  //  power_usart0_enable();     // Serial 0 (USART)
  //  power_usart1_enable();     // Serial 1 (USART)
  //  power_usart2_enable();     // Serial 2 (USART)
  //  power_usart3_enable();     // Serial 3 (USART)
  //  //power_timer0_enable();     // Timer 0
  //  power_timer1_enable();     // Timer 1
  //  power_timer2_enable();     // Timer 2
  //  power_twi_enable();        // TWI (I2C)

  power_all_enable();

  if (wdt_interrupt_flag)
    return 1;
  else if (external_interrupt_flag)
    return 0;
  return -1;
}
