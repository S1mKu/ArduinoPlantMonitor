#include "lcdfeedback.h"

void LCD_Feedback::blank()
{
    LiquidCrystal::clear();
}

void LCD_Feedback::set_feedback(float temperature, float humidity, int moisture, int brightness)
{
    LiquidCrystal::clear();
    LiquidCrystal::setCursor(0, 0);
    LiquidCrystal::print("T:");
    LiquidCrystal::print(temperature);
    LiquidCrystal::print("  M:");
    LiquidCrystal::print(moisture);
    LiquidCrystal::setCursor(0, 1);
    LiquidCrystal::print("H:");
    LiquidCrystal::print(humidity);
    LiquidCrystal::print("  B:");
    LiquidCrystal::print(brightness);
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

void LCD_Feedback::on()
{
    _is_on = true;
    digitalWrite(_backlight_ctl_pin, HIGH);
    LiquidCrystal::display();
}

void LCD_Feedback::off()
{
    _is_on = false;
    LiquidCrystal::noDisplay();
    digitalWrite(_backlight_ctl_pin, LOW);
}

boolean LCD_Feedback::is_on()
{
    return _is_on;
}
