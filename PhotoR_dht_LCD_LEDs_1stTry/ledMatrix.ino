#include "LedControl.h"

LedControl lc=LedControl(24,23,22,2);  // Pins: DIN,CLK,CS, # of Display connected

unsigned long delayTime=2000;  // Delay between Frames

// Put values in arrays
byte mouthArray1[] =
{
  B00000010,  // First frame of invader #1
  B00000110,
  B00001010,
  B00010010,
  B00100010,
  B01000010,
  B10000010,
  B10000010
};

byte mouthArray2[] =
{
  B10000010, // Second frame of invader #1
  B10000010,
  B01000010,
  B00100010,
  B00010010,
  B00001010,
  B00000110,
  B00000010
};

byte smileArray1[] =
{
  B00000010, // First frame of invader #2
  B00000100,
  B00001100,
  B00011000,
  B00110000,
  B00110000,
  B01100000,
  B01100000
};

byte smileArray2[] =
{
  B01100000, // Second frame of invader #2
  B01100000,
  B00110000,
  B00110000,
  B00011000,
  B00001100,
  B00000100,
  B00000010
};

byte sadArray1[] =
{
  B01000000, // First frame of invader #2
  B00100000,
  B00110000,
  B00011000,
  B00001100,
  B00001100,
  B00000110,
  B00000110
};

byte sadArray2[] =
{
  B00000110, // Second frame of invader #2
  B00000110,
  B00001100,
  B00001100,
  B00011000,
  B00110000,
  B00100000,
  B01000000
};

void LEDmatrixSetup()
{
  lc.shutdown(0,false);  // Wake up displays
  lc.shutdown(1,false);
  lc.setIntensity(0,5);  // Set intensity levels
  lc.setIntensity(1,5);
  lc.clearDisplay(0);  // Clear Displays
  lc.clearDisplay(1);
}


//  Take values in Arrays and Display them
void openMouth()
{
  for (int i = 0; i < 8; i++)  
  {
    lc.setRow(0,i,mouthArray1[i]);
  }
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(1,i,mouthArray2[i]);
  }
}

void smile()
{
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(0,i,smileArray1[i]);
  }
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(1,i,smileArray2[i]);
  }
}

void setSadFace()
{
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(0,i,sadArray1[i]);
  }
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(1,i,sadArray2[i]);
  }
}


void LEDMatrixSmileLoop()
{
  int counter = 0;
  while (counter<5) {
    // Put #1 frame on both Display
    openMouth();
    delay(delayTime);
    // Put #2 frame on both Display
    smile(); 
    delay(delayTime);
    counter++;
  }
}                           

void LEDMatrixSadLoop()
{
  int counter = 0;
  while (counter<10) {
    setSadFace();
    delay(delayTime);
    counter++;
  }
}
