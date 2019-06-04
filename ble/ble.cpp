//#include "ble.h"
//
//char buffer[BUFFER_LEN];
//
//unsigned int RxD;
//unsigned int TxD;
//
//SoftwareSerial* blueToothSerial; //the software serial port
//
////send command to Bluetooth and return if there is a response
//int sendBlueToothCommand(char command[])
//{
//  Serial.print("send: ");
//  Serial.print(command);
//  Serial.println("");
//
//  blueToothSerial->print(command);
//  delay(200);
//
//  if (ble_recv(buffer, BUFFER_LEN) <= 0) return -1;
//
//  Serial.print("recv: ");
//  Serial.print(buffer);
//  Serial.println("");
//  return 0;
//}
//
////configure the Bluetooth through AT commands
//int setupBlueToothConnection()
//{
//#if MASTER
//  Serial.println("this is MASTER\r\n");
//#else
//  Serial.println("this is SLAVE\r\n");
//#endif
//
//  Serial.print("Setting up Bluetooth link\r\n");
//  delay(3500);//wait for module restart
//
//  Serial.println("-----------");
//
//  //send command to module in different baud rate
//  while (1)
//  {
//    Serial.println("..................");
//
//    delay(500);
//    blueToothSerial->begin(9600);
//    delay(500);
//    Serial.print("try 9600\r\n");
//    if (sendBlueToothCommand("AT") == 0)
//      break;
//    delay(500);
//    blueToothSerial->begin(115200);
//    delay(500);
//    Serial.print("try 115200\r\n");
//    if (sendBlueToothCommand("AT") == 0)
//      break;
//  }
//
//  Serial.println("::::::::::::::::");
//
//  //we have to set the baud rate to 9600, since the soft serial is not stable at 115200
//  sendBlueToothCommand("AT+RENEW");         //restore factory configurations
//  sendBlueToothCommand("AT+BAUD2");         //reset the module's baud rate
//  sendBlueToothCommand("AT+AUTH1");         //enable authentication
//  sendBlueToothCommand("AT+RESET");         //restart module to take effect
//
//  blueToothSerial->begin(9600);              //reset the Arduino's baud rate
//  delay(3500);                              //wait for module restart
//
//  //configure parameters of the module
//  sendBlueToothCommand("AT+VERS?");         //get firmware version
//  sendBlueToothCommand("AT+ADDE?");         //get EDR MAC
//  sendBlueToothCommand("AT+ADDB?");         //get BLE MAC
//  sendBlueToothCommand("AT+NAMEHM-13-EDR"); //set EDR name
//  sendBlueToothCommand("AT+NAMBHM-13-BLE"); //set BLE name
//  sendBlueToothCommand("AT+PINE123451");    //set EDR password
//  sendBlueToothCommand("AT+PINB123451");    //set BLE password
//  sendBlueToothCommand("AT+SCAN0");         //set module visible
//  sendBlueToothCommand("AT+NOTI1");         //enable connect notifications
//  sendBlueToothCommand("AT+NOTP1");       //enable address notifications
//  sendBlueToothCommand("AT+PIO01");         //enable key function
//#if MASTER
//  sendBlueToothCommand("AT+ROLB1");         //set to master mode
//#else
//  sendBlueToothCommand("AT+ROLB0");         //set to slave mode
//#endif
//  sendBlueToothCommand("AT+RESET");         //restart module to take effect
//  delay(3500);                              //wait for module restart
//  if (sendBlueToothCommand("AT") != 0) return -1; //detect if the module exists
//  Serial.print("Setup complete\r\n\r\n");
//  return 0;
//}
//
///**
//   returns the number of characters read and stored into recv_str (max 100)
//*/
//int ble_recv(char *buffer, unsigned int buffer_len, unsigned int timeout = STD_RCV_TIMEOUT)
//{
//  //wait for feedback
//  unsigned int time = 0;
//  unsigned char i;
//
//  //Serial.print("STOP");
//
//  //waiting for the first character with time out
//  i = 0;
//  while (1)
//  {
//    delay(50);
//    if (blueToothSerial->available())
//    {
//      buffer[i] = char(blueToothSerial->read());
//      i++;
//      break;
//    }
//    time++;
//    if (time > (timeout / 50)) return 0;
//  }
//
//  //Serial.println("  -> CONTINUE: ");
//  //delay(200);
//
//  //TODO handle buffer overflow -> software serial buffer can only hold up to 64 bytes
//
//  //read other characters from uart buffer to string
//  while (blueToothSerial->available() && (i < buffer_len - 1))
//  {
//    buffer[i] = char(blueToothSerial->read());
//    i++;
//  }
//  buffer[i] = '\0';
//
//  return i;
//}
//
////int ble_send(char *msg, unsigned int msg_len)
////{
////  blueToothSerial->print(msg, msg_len);
////}
//
//int ble_send(char msg[])
//{
//  blueToothSerial->print(msg);
//}
//
//void ble_setup(unsigned int pin_RxD = STD_RxD, unsigned int pin_TxD = STD_TxD)
//{
//  RxD = pin_RxD;
//  TxD = pin_TxD;
//
//  //Serial.begin(9600);
//
//  pinMode(RxD, INPUT);    //UART pin for Bluetooth
//  pinMode(TxD, OUTPUT);   //UART pin for Bluetooth
//
//  blueToothSerial = &SoftwareSerial(RxD, TxD); //the software serial port
//
//  if (setupBlueToothConnection() != 0) while (1); //initialize Bluetooth
//}
//
//bool ble_connected(void)
//{
//  int bytes_received = ble_recv(buffer, BUFFER_LEN);
//
//  if (bytes_received > 0 && strstr(buffer, (char *)"CON") != NULL)
//  {
//    Serial.print("buffer: ");
//    Serial.print(buffer);
//    Serial.print("\n");
//    return true;
//  }
//
//  return false;
//}
