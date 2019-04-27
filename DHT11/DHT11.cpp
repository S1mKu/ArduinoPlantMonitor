#include "Arduino.h"
#include "DHT11.h"

DHT11::DHT11(const char *str) {
	Serial.begin(9600);
	_str = str;
}

void DHT11::some_function() {
	Serial.println(_str);
}
