#include "sleep.h"

void setup(void)
{
  Serial.begin(9600);
  configure_sleep_mode(100);
}

void loop(void)
{
  Serial.println("going to sleep");
  delay(200);

  int interrupt_cause = enter_sleep_mode();

  Serial.println("woke up again");

  if (interrupt_cause == 0)
    Serial.println("EXTERNAL INTERRUPT ON PIN 2");
  else if (interrupt_cause == 1)
    Serial.println("WDT INTERRUPT");
  else
    Serial.println("UNKNOWN INTERRUPT CAUSE");
}
