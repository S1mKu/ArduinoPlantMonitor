

# ArduinoPlantMonitor
A plant monitoring system based on Arduino.

# Sensors
1. Arduino UNO
2. LCD
3. DHT11
4. LED-Matrix
5. moisture sensor
6. DC battery (9V)
7. photocell
8. RTC
8. SD card adapter

> 8. stepper motor (turns plant depending on sunlight direction, 24h)
> 9. bluetooth / WIFI / cable for data transmission, e.g. temp/moisture
gradient/graph (eval. and displayed on mobile device/laptop/pc)
> 10. RPi + webcam for timelap
> 11. SD card for data storage/buffer + sound?
>     11.1 loudspeakers/amplifier + sound sensor
> 12. PIR Motion Sensor (generic)

## Module/Hardware unit pinout
### Soil Moisture Sensor for Arduino (ME110)
| Moisture Sensor pin | Arduino pin |
|---|---|
| + | 5V |
| - | GND |
| S |  (any analog output pin) |

0~3.0V when 5V power supply; 0~1.7V when 3.3V power supply

### LCD 1602 Module
| LCD pin | Arduino Mega pin |
|---|---|
| RS | D12 |
| Enable | D11 |
| D4 | D5 |
| D5 | D4 |
| D6 | D3 |
| D7 | D2 |

Additionally, wire a 10k pot to **+5V** and **GND**, with it's wiper (output) to LCD screens **VO** pin (pin3). A 220 ohm resistor is used to power the backlight of the display, usually on **pin 15 and 16 of the LCD** connector

### DHT11 (three pins)
| DHT11 pin | Arduino Mega pin |
|---|---|
| Signal | D7 |
| Vcc | 5V |
| Ground | GND |

### MAX7219 Module (LED-Matrix)
| MAX7219 pin | Arduino Mega pin |
|---|---|
| Vcc | 5V |
| GND | GND |
| DIN | D51 (MOSI) |
| CS | D53 (SS/CS) |
| CLK | D52 |

Use library 'LedControl'!

### [Grove-BLE dual model v1.0](http://wiki.seeedstudio.com/Grove-BLE-dual_model-v1.0/)
| Grove BLE | Aduino Mega pin |
|---|---|
| GND | GND |
| VCC | 5V (3.3V also possible?) |
| RX | D1 (TX) |
|TX | D0 (RX) |

+3.3/+5.0VDC 50mA

## Arduino Mega 2560 Pinout

<table>
   <tr>
      <th colspan="2">Arduino pinout</th>
      <th rowspan="2">Module pinout</th>
      <th rowspan="2">Module/Hardware unit</th>
   </tr>
   <tr>
      <th>Number</th>
      <th>Name</th>
   </tr>
   <tr>
      <td>0</td>
      <td>RX</td>
      <td>H</td>
      <td>I</td>
   </tr>
   <tr>
      <td>1</td>
      <td>TX</td>
      <td>H</td>
      <td>I</td>
   </tr>
</table>

## MULTIPLEXER WITH ARDUINO
There are many different opportunity: 
1.    8 channels multiplexer CD4051BE
2.    8 pin shift register 74HC595
3.    8-channel analog multiplexer/demultiplexer 74HC4051
