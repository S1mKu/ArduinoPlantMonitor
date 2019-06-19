#ifndef _LCDFEEDBACK_H_
#define _LCDFEEDBACK_H_

#include <LiquidCrystal.h>
#include <stdint.h>

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
        begin(16, 2);
    };

    void blank();
    void set_feedback(int16_t temperature, int16_t humidity);
    void display(uint8_t text[], uint16_t text_len, uint16_t cursor_line, uint16_t cursor_col, bool clear = false);

}

//check the pins
LiquidCrystal lcd(42, 41, 40, 39, 38, 37); //(rs, en, d4, d5, d6, d7);


#endif