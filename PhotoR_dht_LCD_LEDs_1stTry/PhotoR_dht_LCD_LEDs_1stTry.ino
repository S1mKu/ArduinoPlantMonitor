//angeschlossen mit 330Ohm
// Variables

void setup() {
  // setups from all components
  //for test reasons: 
  //Serial.begin(115200);
  lcdBegin();
  photoSellSetup();
  LEDmatrixSetup();
}

void loop() {
  LCDclear();
  
  checkDataFromDHT();

  LCDsetCursor(0,0);
  LCDprintString("Temp: ");
  LCDprintInt(getTemperature());
  LCDprintChar((char)223);
  LCDprintString("C");

  LCDsetCursor(0,1);
  LCDprintString("Humidity: ");
  LCDprintInt(getHumidity());
  LCDprintString("%");
  
  //
  
  LEDMatrixSmileLoop();
  
  LCDclear();
  
  if (getPhotocellValue() > 800 && moistureSensorValue() > 400) {
    LCDsetCursor(0,0);
    LCDprintString("Enough light and ");
    LCDsetCursor(0,1);
    LCDprintString("I don't need water ;)");
    LEDMatrixSmileLoop();
  } else if (getPhotocellValue() < 800 && moistureSensorValue() > 400 ) {
    LCDsetCursor(0,0);
    LCDprintString("Too dark :(");
    LEDMatrixSadLoop();
  } else if (getPhotocellValue() > 800 && moistureSensorValue() < 400 ) {
    LCDsetCursor(0,0);
    LCDprintString("I need water :(");
    LEDMatrixSadLoop();
  } else {
    LCDsetCursor(0,0);
    LCDprintString("U don't love me :(");
    LEDMatrixSadLoop(); 
  }

  LEDMatrixSmileLoop();
  
  //Serial.println(getPhotocellValue());
  /*
   * for test reason
  Serial.println("Temperature = ");
  Serial.println(getTemperature());
  Serial.print("Humidity = ");
  Serial.print(getHumidity());
  Serial.println("%");
  */
}
