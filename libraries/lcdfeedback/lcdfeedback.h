#ifndef _LCDFEEDBACK_H_
#define _LCDFEEDBACK_H_

#include <LiquidCrystal.h>
#include <stdint.h>
#include <Arduino.h>

class LCD_Feedback : protected LiquidCrystal
{
private:
    boolean _is_on = false;
    uint16_t _backlight_ctl_pin;

public:
    LCD_Feedback(
        uint16_t rsPin,
        uint16_t enPin,
        uint16_t d4Pin,
        uint16_t d5Pin,
        uint16_t d6Pin,
        uint16_t d7Pin,
        uint16_t backlight_ctl_pin) : _backlight_ctl_pin(backlight_ctl_pin), LiquidCrystal(rsPin, enPin, d4Pin, d5Pin, d6Pin, d7Pin)
    {
        LiquidCrystal::begin(16, 2);
        LiquidCrystal::noDisplay();
        pinMode(_backlight_ctl_pin, OUTPUT);
        digitalWrite(_backlight_ctl_pin, LOW);
    };

    void blank();
    void set_feedback(float temperature, float humidity, int moisture, int brightness);
    void display(const char text[], uint16_t text_len, uint16_t cursor_line, uint16_t cursor_col, bool clear = false);

    void on();
    void off();
    boolean is_on();
};

#endif