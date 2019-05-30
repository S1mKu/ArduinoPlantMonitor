/* Use a photoresistor (or photocell) to turn on an LED in the dark
   More info and circuit schematic: http://www.ardumotive.com/how-to-use-a-photoresistor-en.html
   Dev: Michalis Vasilakis // Date: 8/6/2015 // www.ardumotive.com */

//angeschlossen mit 2x 10KOhm

//Constants
const int pResistor = A1; // Photoresistor at Arduino analog pin A0
//const int ledPin=9;       // Led pin at Arduino pin 9

void photoSellSetup(){
 //pinMode(ledPin, OUTPUT);  // Set lepPin - 9 pin as an output
 pinMode(pResistor, INPUT);// Set pResistor - A0 pin as an input (optional)
}

int getPhotocellValue() {
  return analogRead(pResistor);
}
