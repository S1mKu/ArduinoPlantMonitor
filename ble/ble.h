#ifndef BLE_PLANT_MONITOR_H
#define BLE_PLANT_MONITOR_H

#include <SoftwareSerial.h>
#include <string.h>

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define STD_RxD (11)
#define STD_TxD (10)

#define BUFFER_LEN (100)

extern char buffer[BUFFER_LEN];

#define MASTER (1) //change this macro to define the Bluetooth as Master or not

#define STD_RCV_TIMEOUT (1000) // 1 second timeout before cancelling rcv process

// extern char rcv_str[100];

/**
  returns the number of characters read and stored into buffer

  Be aware though, the software serial buffer can only hold up to 64 bytes.
*/
int ble_recv(char *buffer, unsigned int buffer_len, unsigned int timeout = STD_RCV_TIMEOUT);

//int ble_send(char *msg, unsigned int msg_len);
int ble_send(char msg[]);

void ble_setup(unsigned int pin_RxD = STD_RxD, unsigned int pin_TxD = STD_TxD);

bool ble_connected(void);

#endif
