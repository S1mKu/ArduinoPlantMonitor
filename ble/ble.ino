#include "ble.h"

void setup(void)
{
  Serial.begin(9600);

  ble_setup();

  Serial.println("####################");
}

unsigned int i = 1;

void loop(void)
{
  //ble_send("AT+RESET");
  
  if (ble_connected())
  {

    Serial.println("###### CONNECTED ######");

    //char msg[] = "testnachricht";
  
    ble_send(msg, strlen(msg));

    int msg_len = ble_recv(buffer, BUFFER_LEN); // using default timeout of 1 second

    if (msg_len)
      Serial.println(buffer);
    else
      Serial.println("Received nothing.");
  }

  buffer[0] = '\0';

  Serial.print(".");
  if (i % 10 == 0)
    Serial.print("\n");

  i++;
  delay(200);
}
