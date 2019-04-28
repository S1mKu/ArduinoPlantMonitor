#include <dht.h>
#define DHT11_PIN 7

//check the pins
dht DHT;

int getTemperature() {
  return DHT.temperature;  
}

int getHumidity() {
  return DHT.humidity;
}

int checkDataFromDHT() {
  //read the data from the senor
  int chk = DHT.read11(DHT11_PIN);
}
