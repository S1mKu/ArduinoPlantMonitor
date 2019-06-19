#include "lcdfeedback.h"

void LCD_Feedback::blank()
{
    clear();
}

void LCD_Feedback::set_feedback(int16_t temperature, int16_t humidity)
{
    clear();
    setCursor(0,0);
    print("Temp: ");
    print(temperature);
    setCursor(0,1);
    print("Humidity: ");
    print(humidity);
}

void LCD_Feedback::display(uint8_t text[], uint16_t text_len, uint16_t cursor_col, uint16_t cursor_line, bool clear = false)
{
    if (clear)
    {
        clear();
    }

    setCursor(cursor_col, cursor_line);
    print(text);
}
