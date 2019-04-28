
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
  
  LEDMatrixLoop();
  
  LCDclear();
  LCDsetCursor(0,0);

  if (getPhotocellValue() > 800) {
    LCDprintString("Enough light :)");
  } else {
    LCDprintString("Too dark :(");
  }

  LEDMatrixLoop();
  
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
