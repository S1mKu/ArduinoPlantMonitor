#include "sleep.h"
#include "timer.h"

TimeoutInterval interval;

/**
  @brief   Flag stating if sleep has been entered or not.
*/
volatile bool _ble_interrupt_flag = false;

unsigned int _ble_interrupt_pin;

void BLE_INTERRUPT_ISR(void)
{
    detachInterrupt(digitalPinToInterrupt(_ble_interrupt_pin));
    _ble_interrupt_flag = true;
}

void configure_sleep_mode(TimeoutInterval interval,
                          unsigned int timer_interrupt_pin = STD_TIMER_INTERRUPT_PIN,
                          unsigned int ble_interrupt_pin = STD_BLE_INTERRUPT_PIN)
{
    _ble_interrupt_pin = ble_interrupt_pin;
    pinMode(_ble_interrupt_pin, INPUT);
    TIMER.set_interval(interval);
}

void setup_sleep_interrupts(void)
{
    //attachInterrupt(digitalPinToInterrupt(_ble_interrupt_pin), BLE_INTERRUPT_ISR, RISING);
    TIMER.begin();
}

void _shutdown(void)
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // power_adc_disable();    // ADC converter
    // power_spi_disable();    // SPI
    // power_usart0_disable(); // Serial 0 (USART)
    // power_usart1_disable(); // Serial 1 (USART)
    // power_usart2_disable(); // Serial 2 (USART)
    // power_usart3_disable(); // Serial 3 (USART)
    // //power_timer0_disable();   // Timer 0
    // power_timer1_disable(); // Timer 1
    // power_timer2_disable(); // Timer 2
    // power_twi_disable();    // TWI (I2C)

    Serial.println("... shutdown");
}

/**
   returns 0 for an external interrupt and 1 for a wdt interrupt and -1 otherwise
*/
int enter_sleep_mode(void)
{
    //_shutdown();

    /* reset flags and counters - do not give any interrupt the chance to occure before sleeping */
    // cli();

    // if (_alarm_flag)
    // {
    //     _alarm_flag = false;
    //     sei();

    //     Serial.println("oooooooo");
    //     Serial.println(_alarm_flag);

    //     //TIMER.set_next_alarm();
    //     power_all_enable();
    //     return 1;
    // }

    // if (_ble_interrupt_flag)
    // {
    //     _ble_interrupt_flag = false;
    //     sei();
    //     power_all_enable();

    //     /* Setup INT_PIN as an interrupt and attach handler. */
    //     attachInterrupt(digitalPinToInterrupt(_ble_interrupt_pin), BLE_INTERRUPT_ISR, HIGH);
    //     return 0;
    // }

    // sleep_enable();
    // sei();

    // /* command after sei() will be executed before any interrupt can occure */
    // sleep_mode();

    // /* Execution will resume here. */
    // sleep_disable();

    // power_all_enable();

    // Serial.println("hhhhhhhhhh");
    // delay(100);

    if (_alarm_flag)
    {
        Serial.println("lllllllll");
        delay(100);

        _alarm_flag = false;
        TIMER.set_next_alarm();
        return 1;
    }
    else if (_ble_interrupt_flag)
    {
        Serial.println("iiiiiiiii");
        _ble_interrupt_flag = false;
        return 0;
    }
    return -1;
}
