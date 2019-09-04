# riverNet

This sensor package allows users to take measurements from multiple sensors, read the measurements, and write the data to an SD card, all in a matter of seconds. It implements a sleep and alarm mode that enables measurements to be taken at defined intervals.

Implemented in Arduino, C++. 

Libraries used: 
- Vector.h - From Arduino IDE
- Time.h - https://github.com/PaulStoffregen/Time
- LowPower.h - https://github.com/rocketscream/Low-Power
- PCF8523.h - https://github.com/SpellFoundry/PCF8523
- SleepyPi.h - https://github.com/SpellFoundry/SleepyPi2

The following sensors have been implemented thus far: 
  1. Atlas Temperature sensor
  2. Atlas Dissolved Oxygen sensor 
  3. Atlas Flow Rate sensor 
  4. Catnip Electronics Soil Moisture sensor 
  
The implementation of the sensors allow for easy removal of sensors that are unwanted, as well as addition of sensors as needed. 

Structure: 
- A virtual class that is comprised of sensors 
- All other classes represent an implemented sensor that inherits virtual class functions  
- Array of sensor classes to access the data recordings. 

Hardware:
Pin Setup: 
- Interrupt Pin: 2 - for sleep/alarm mode 
- I^2C: SDA, SCL - to connect sensors using only 2 pins 
- GND
- 5v
Note: Different pin usage must be reflected in the source code

Design Decisions:
- We initially implemented the sensor pod using the Arduino/ Genuino Uno; however, we realized that the board consumed too much battery power. We could not accept this since our constraints required the battery to power the pod for approximately 6 months. This led us to switching to the Arduino pro mini (5V), which supposedly consumes less power. 
- The majority of our sensors are manufactured by Atlas. We decided to use these sensors rather than those manufactured by Vernier was decided as they are much simpler to use and provide us with more accurate readings. Difficulties in configuring the Vernier temperature probe and limitations with the Vernier flow rate sensor specifically led us to deciding on using the Atlas sensors instead. 
- Instead of wiring the sensors on a breadboard, we decided to connect everything using our own printed circuit board (PCB). This makes the connections more robust ensuring that no wire would come unplugged (wires on a breadboard would be especially vulnerable to currents in the river) 
