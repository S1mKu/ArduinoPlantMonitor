#include "lcdfeedback.h"

void LCD_Feedback::blank()
{
    LiquidCrystal::clear();
}

void LCD_Feedback::set_feedback(int16_t temperature, int16_t humidity)
{
    LiquidCrystal::clear();
    LiquidCrystal::setCursor(0,0);
    LiquidCrystal::print("Temp: ");
    LiquidCrystal::print(temperature);
    LiquidCrystal::setCursor(0,1);
    LiquidCrystal::print("Humidity: ");
    LiquidCrystal::print(humidity);
}

void LCD_Feedback::display(const char text[], uint16_t text_len, uint16_t cursor_col, uint16_t cursor_line, bool clear = false)
{
    if (clear)
    {
        LiquidCrystal::clear();
    }

    LiquidCrystal::setCursor(cursor_col, cursor_line);
    LiquidCrystal::print(text);
}
