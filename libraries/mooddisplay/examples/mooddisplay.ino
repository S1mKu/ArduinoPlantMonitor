#include "mooddisplay.h"

MoodDisplay mood_display = MoodDisplay(24, 23, 22);

void setup()
{
    mood_display.setup();
}

void loop()
{
    delay(2000);

    mood_display.smile();

    delay(2000);

    mood_display.cry();
}
