int sensorPin = A2;

int moistureSensorValue() {
  //Werte zw 0 und 1023
  return analogRead(sensorPin);
}
