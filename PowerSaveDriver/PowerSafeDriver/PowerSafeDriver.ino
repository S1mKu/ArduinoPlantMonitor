#include <stdbool.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include <SoftwareSerial.h>

#define RxD 11
#define TxD 10

#define MASTER 0    //change this macro to define the Bluetooth as Master or not 

SoftwareSerial blueToothSerial(RxD, TxD); //the software serial port

char recv_str[100];

#define EXT_INT_PIN 2
#define LED_PIN 13

/**
   @brief   Flag stating if sleep has been entered or not.
*/
volatile bool sleep_entered;
volatile bool external_interrupt_flag = false;
volatile bool wdt_interrupt_flag = false;

volatile int wdt_interrupt_counter = 0;

int wdt_interrupt_limit = 8;

volatile int i = 0;
volatile int ii = 0;

unsigned long start_time;
unsigned long end_time;

/**
   @brief We need to enable the WDT for Interrupt mode only, so we can use it
          as a sleep wakeup timer as well as for detecting lockups.
   @note  Inline function!
*/
inline void configure_wdt(void)
{
  /* A 'timed' sequence is required for configuring the WDT, so we need to
     disable interrupts here.
  */
  cli();
  wdt_reset();
  MCUSR &= ~_BV(WDRF);
  /* Start the WDT Config change sequence. */
  WDTCSR |= _BV(WDCE) | _BV(WDE);
  /* Configure the prescaler and the WDT for interrupt mode only*/
  //WDTCSR =  _BV(WDP1) | _BV(WDP2) | _BV(WDIE);
  WDTCSR =  _BV(WDP0) | _BV(WDP3) | _BV(WDIE);
  sei();
}

void try_sleep(void)
{
  Serial.println("try_sleep");
  delay(100);

  //configure_wdt();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  //set_sleep_mode(SLEEP_MODE_PWR_SAVE);

  power_adc_disable(); // ADC converter
  power_spi_disable(); // SPI
  power_usart0_disable();// Serial 0 (USART)
  power_usart1_disable();// Serial 1 (USART)
  power_usart2_disable();// Serial 2 (USART)
  power_usart3_disable();// Serial 3 (USART)
  //power_timer0_disable();// Timer 0
  power_timer1_disable();// Timer 1
  power_timer2_disable();// Timer 2
  power_twi_disable(); // TWI (I2C)

  cli();

  if (external_interrupt_flag || wdt_interrupt_flag)
  {
    sei();
    return;
  }

  /* Setup INT_PIN as an interrupt and attach handler. */
  attachInterrupt(digitalPinToInterrupt(EXT_INT_PIN), EXT_ISR, LOW);

  sleep_entered = true;
  sleep_enable();

  // turn off brown-out enable in software
  //  MCUCR = bit (BODS) | bit (BODSE);
  //  MCUCR = bit (BODS);

  sei();

  sleep_mode();
  /* Execution will resume here. */
  sleep_disable();

  power_adc_enable(); // ADC converter
  power_spi_enable(); // SPI
  power_usart0_enable();// Serial 0 (USART)
  power_usart1_enable();// Serial 1 (USART)
  power_usart2_enable();// Serial 2 (USART)
  power_usart3_enable();// Serial 3 (USART)
  //power_timer0_enable(); // Timer 0
  power_timer1_enable(); // Timer 1
  power_timer2_enable(); // Timer 2
  power_twi_enable(); // TWI (I2C)

  Serial.println("returned from sleep");
  delay(200);
}

/**
   @brief   The watchdog interrupt. The watchdog has two functions: 1/ to detect
            if the application code has locked up. 2/ to wake up the module
            from sleep.
*/
ISR(WDT_vect)
{
  wdt_interrupt_counter++;
  wdt_interrupt_counter = wdt_interrupt_counter % wdt_interrupt_limit;

  if (wdt_interrupt_counter == 0)
  {
    wdt_interrupt_flag = true;
  }
}

void EXT_ISR(void)
{
  detachInterrupt(digitalPinToInterrupt(EXT_INT_PIN));
  external_interrupt_flag = true;
}

//used for compare two string, return 0 if one equals to each other
int strcmp(char *a, char *b)
{
  unsigned int ptr = 0;
  while (a[ptr] != '\0')
  {
    if (a[ptr] != b[ptr]) return -1;
    ptr++;
  }
  return 0;
}

//configure the Bluetooth through AT commands
int setupBlueToothConnection()
{
#if MASTER
  Serial.println("this is MASTER\r\n");
#else
  Serial.println("this is SLAVE\r\n");
#endif

  Serial.print("Setting up Bluetooth link\r\n");
  delay(3500);//wait for module restart

  //send command to module in different baud rate
  while (1)
  {
    delay(500);
    blueToothSerial.begin(9600);
    delay(500);
    Serial.print("try 9600\r\n");
    if (sendBlueToothCommand("AT") == 0)
      break;
    delay(500);
    blueToothSerial.begin(115200);
    delay(500);
    Serial.print("try 115200\r\n");
    if (sendBlueToothCommand("AT") == 0)
      break;
  }

  //we have to set the baud rate to 9600, since the soft serial is not stable at 115200
  sendBlueToothCommand("AT+RENEW");//restore factory configurations
  sendBlueToothCommand("AT+BAUD2");//reset the module's baud rate
  sendBlueToothCommand("AT+AUTH1");//enable authentication
  sendBlueToothCommand("AT+RESET");//restart module to take effect
  blueToothSerial.begin(9600);//reset the Arduino's baud rate
  delay(3500);//wait for module restart
  //configure parameters of the module
  sendBlueToothCommand("AT+VERS?");//get firmware version
  sendBlueToothCommand("AT+ADDE?");//get EDR MAC
  sendBlueToothCommand("AT+ADDB?");//get BLE MAC
  sendBlueToothCommand("AT+NAMEHM-13-EDR");//set EDR name
  sendBlueToothCommand("AT+NAMBHM-13-BLE");//set BLE name
  sendBlueToothCommand("AT+PINE123451");//set EDR password
  sendBlueToothCommand("AT+PINB123451");//set BLE password
  sendBlueToothCommand("AT+SCAN0");//set module visible
  sendBlueToothCommand("AT+NOTI1");//enable connect notifications
  //sendBlueToothCommand("AT+NOTP1");//enable address notifications
  sendBlueToothCommand("AT+PIO01");//enable key function
#if MASTER
  sendBlueToothCommand("AT+ROLB1");//set to master mode
#else
  sendBlueToothCommand("AT+ROLB0");//set to slave mode
#endif
  sendBlueToothCommand("AT+RESET");//restart module to take effect
  delay(3500);//wait for module restart
  if (sendBlueToothCommand("AT") != 0) return -1; //detect if the module exists
  Serial.print("Setup complete\r\n\r\n");
  return 0;
}

//send command to Bluetooth and return if there is a response
int sendBlueToothCommand(char command[])
{
  Serial.print("send: ");
  Serial.print(command);
  Serial.println("");

  blueToothSerial.print(command);
  delay(200);

  if (recvMsg(200) != 0) return -1;

  Serial.print("recv: ");
  Serial.print(recv_str);
  Serial.println("");
  return 0;
}

//receive message from Bluetooth with time out
int recvMsg(unsigned int timeout)
{
  //wait for feedback
  unsigned int time = 0;
  unsigned char num;
  unsigned char i;

  //waiting for the first character with time out
  i = 0;
  while (1)
  {
    delay(50);
    if (blueToothSerial.available())
    {
      recv_str[i] = char(blueToothSerial.read());
      i++;
      break;
    }
    time++;
    if (time > (timeout / 50)) return -1;
  }

  //read other characters from uart buffer to string
  while (blueToothSerial.available() && (i < 100))
  {
    recv_str[i] = char(blueToothSerial.read());
    i++;
  }
  recv_str[i] = '\0';

  return 0;
}

void setup()
{
  Serial.begin(9600);

  pinMode(RxD, INPUT);    //UART pin for Bluetooth
  pinMode(TxD, OUTPUT);   //UART pin for Bluetooth
  Serial.println("\r\nPower on!");

  if (setupBlueToothConnection() != 0) while (1); //initialize Bluetooth

  configure_wdt();
  wdt_reset();
}

void loop()
{
  /* reset flags and counters */
  {
    cli();

    if (wdt_interrupt_flag)
    {
      wdt_interrupt_counter = 0;
      wdt_interrupt_flag = false;
    }

    if (external_interrupt_flag)
    {
      external_interrupt_flag = false;
    }

    sei();
  }

  try_sleep();

  if (wdt_interrupt_flag)
  {
    Serial.println("WDT INTERRUPT\n");
  }

  if (external_interrupt_flag)
  {
    Serial.println("EXTERNAL INTERRUPT\n");

    if (blueToothSerial.available() && recvMsg(2000) == 0)
    {
      Serial.print("recv: ");
      Serial.print((char *)recv_str);
      Serial.println("");
      Serial.println("send: hello");
      blueToothSerial.print("hello");//return back message
      blueToothSerial.flush();
      delay(200);
    } else {
      Serial.println("send: ...");
      blueToothSerial.print("...");//return back message
      blueToothSerial.flush();
      delay(200);
      Serial.println("ERROR");
    }
  }

  Serial.println("###############################");
  delay(200);
}




/*
  WDT interrupt every X seconds:

  if ( interrupt count limit not passed )
  {
    increment interrupt counter by one;
  }
  else // interrupt count limit passed
  {
    if ( sleep entered )
    {

    }
  }


*/
