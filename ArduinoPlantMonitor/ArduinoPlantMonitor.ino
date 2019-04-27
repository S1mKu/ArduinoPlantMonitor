#include <DHT11.h>

DHT11 dht11("Some text to output in the Arduino IDE Serial Monitor window.");

void setup() {}

void loop(){
  dht11.some_function();
  delay(2000);
}
