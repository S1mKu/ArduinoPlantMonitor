#ifndef _LCDFEEDBACK_H_
#define _LCDFEEDBACK_H_

#include <LiquidCrystal.h>
#include <stdint.h>
#include <Arduino.h>

class LCD_Feedback : protected LiquidCrystal
{
public:
    LCD_Feedback(
        uint16_t rsPin,
        uint16_t enPin,
        uint16_t d4Pin,
        uint16_t d5Pin,
        uint16_t d6Pin,
        uint16_t d7Pin) : LiquidCrystal(rsPin, enPin, d4Pin, d5Pin, d6Pin, d7Pin)
    {
        LiquidCrystal::begin(16, 2);
    };

    void blank();
    void set_feedback(int16_t temperature, int16_t humidity);
    void display(const char text[], uint16_t text_len, uint16_t cursor_line, uint16_t cursor_col, bool clear = false);

};

#endif