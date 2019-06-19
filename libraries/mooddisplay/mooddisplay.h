#ifndef _MOODDISPLAY_H_
#define _MOODDISPLAY_H_

#include "LedControl.h"
#include <stdint.h>

class MoodDisplay : protected LedControl
{
private:
    unsigned long delayTime = 2000; // Delay between Frames

    // Put values in arrays
    uint8_t mouthArray1[8] =
        {
            B00000010, // First frame of invader #1
            B00000110,
            B00001010,
            B00010010,
            B00100010,
            B01000010,
            B10000010,
            B10000010};

    uint8_t mouthArray2[8] =
        {
            B10000010, // Second frame of invader #1
            B10000010,
            B01000010,
            B00100010,
            B00010010,
            B00001010,
            B00000110,
            B00000010};

    uint8_t smileArray1[8] =
        {
            B00000010, // First frame of invader #2
            B00000100,
            B00001100,
            B00011000,
            B00110000,
            B00110000,
            B01100000,
            B01100000};

    byte smileArray2[8] =
        {
            B01100000, // Second frame of invader #2
            B01100000,
            B00110000,
            B00110000,
            B00011000,
            B00001100,
            B00000100,
            B00000010};

    uint8_t sadArray1[8] =
        {
            B01000000, // First frame of invader #2
            B00100000,
            B00110000,
            B00011000,
            B00001100,
            B00001100,
            B00000110,
            B00000110};

    uint8_t sadArray2[8] =
        {
            B00000110, // Second frame of invader #2
            B00000110,
            B00001100,
            B00001100,
            B00011000,
            B00110000,
            B00100000,
            B01000000};

    void set_open_mouth();
    void set_smile();
    void set_sad_face();

public:
    MoodDisplay(uint16_t dataPin, uint16_t clkPin, uint16_t csPin) : LedControl(dataPin, clkPin, csPin, 2){};
    void setup();
    void smile();
    void cry();
};

#endif