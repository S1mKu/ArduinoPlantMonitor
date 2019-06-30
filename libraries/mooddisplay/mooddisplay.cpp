#include "mooddisplay.h"

void MoodDisplay::setup()
{
    shutdown(0, false); // Wake up displays
    shutdown(1, false);
    setIntensity(0, 0); // Set intensity levels
    setIntensity(1, 0);
    clearDisplay(0); // Clear Displays
    clearDisplay(1);
}

void MoodDisplay::set_open_mouth()
{
    _mood = 1;
    for (uint16_t i = 0; i < 8; i++)
    {
        setRow(0, i, mouthArray1[i]);
    }
    for (uint16_t i = 0; i < 8; i++)
    {
        setRow(1, i, mouthArray2[i]);
    }
}

void MoodDisplay::set_smile()
{
    _mood = 1;
    for (uint16_t i = 0; i < 8; i++)
    {
        setRow(0, i, smileArray1[i]);
    }
    for (uint16_t i = 0; i < 8; i++)
    {
        setRow(1, i, smileArray2[i]);
    }
}

void MoodDisplay::set_sad_face()
{
    _mood = 2;
    for (uint16_t i = 0; i < 8; i++)
    {
        setRow(0, i, sadArray1[i]);
    }
    for (uint16_t i = 0; i < 8; i++)
    {
        setRow(1, i, sadArray2[i]);
    }
}

void MoodDisplay::play_smile_sequence()
{
    uint16_t counter = 0;
    while (counter < 5)
    {
        // Put #1 frame on both Display
        set_open_mouth();
        delay(delayTime);
        // Put #2 frame on both Display
        set_smile();
        delay(delayTime);
        counter++;
    }
}

void MoodDisplay::play_cry_sequence()
{
    uint16_t counter = 0;
    while (counter < 10)
    {
        set_sad_face();
        delay(delayTime);
        counter++;
    }
}

uint8_t MoodDisplay::get_mood()
{
    return _mood;
}
