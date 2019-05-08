

# ArduinoPlantMonitor
A plant monitoring system based on Arduino.

---

# Open Guideline Suggestions

> Those guidelines are not meant as strict rules, but rather as loose
approach to avoid conflict and confusion. They aim at establishing a
basic common understanding of the projects structure and the workflow.
>
> Feel free to commit your own ideas and comment on the suggestions.

## Directory Structure
1. The repository root represents the sketchbook of the Arduino IDE
2. Every feature/hardware unit is given its own sketch folder in order to
isolate it from other features, each representing its own functional unit
   1. Allows for separate and isolated testing of each feature
   2. If a feature is properly tested, it will be 'converted' to a library
by copying the entire sketch folder into the library directory and
moving all its .ino files into an example subdirectory
   3. Finally, the library gets integrated into the ArduinoPlantMonitor
sketch

## Branching
1. The master branch always includes a properly tested and executable program
2. (Every feature/hardware unit has its own feature branch)?

## Coordination
1. From time to time use the project board so others can see the current
state of the project
   1. Tag your cards/issues with a priority and adjust the "assignee" field
2. Use the Wiki to document topics that others might benefit from

**The branch 'DHT11-example' includes an example of the proposed directory
structure. On the wiki page 'Home' you can find some links about sketches,
the sketchbook, etc.**

---

# Sensors
1. Arduino UNO
2. LCD
3. DHT11
4. LED-Matrix
5. moisture sensor
6. DC battery (9V)
7. photocell

> 8. stepper motor (turns plant depending on sunlight direction, 24h)
> 9. bluetooth / WIFI / cable for data transmission, e.g. temp/moisture
gradient/graph (eval. and displayed on mobile device/laptop/pc)
> 10. RPi + webcam for timelap
> 11. SD card for data storage/buffer + sound?
>     11.1 loudspeakers/amplifier + sound sensor
> 12. PIR Motion Sensor (generic)

