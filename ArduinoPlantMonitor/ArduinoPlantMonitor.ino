#include "ble.h"
#include "sleep.h"

void setup() {
  Serial.begin(9600);

  ble_setup();
  configure_sleep_mode(100);
}

void loop() {
  buffer[0] = '\0';
  
  ble_send("AT+RESET");
  
  Serial.println("going to sleep");
  delay(200);

  int interrupt_cause = enter_sleep_mode();

  Serial.println("woke up again");

  int msg_len = ble_recv(buffer, BUFFER_LEN); // using default timeout of 1 second
  Serial.println(buffer);

  if (interrupt_cause == 0)
  {
    Serial.println("EXTERNAL INTERRUPT ON PIN 2");

    if (ble_connected())
    {

      Serial.println("###### CONNECTED ######");

      char msg[] = "testnachricht";

      ble_send(msg);

      int msg_len = ble_recv(buffer, BUFFER_LEN); // using default timeout of 1 second

      if (msg_len)
        Serial.println(buffer);
      else
        Serial.println("Received nothing.");
    }
  }
  else if (interrupt_cause == 1)
  {
    Serial.println("WDT INTERRUPT");
  }
  else
  {
    Serial.println("UNKNOWN INTERRUPT CAUSE");
  }
}
