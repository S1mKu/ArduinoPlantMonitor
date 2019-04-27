#ifndef DHT11_h
#define DHT11_h

#include "Arduino.h"

class DHT11 {
	public:
		DHT11(const char *str);
		void some_function();
	private:
		const char *_str;
};

#endif
