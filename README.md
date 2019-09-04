# riverNet

The RiverNet package allows you to use various I2C sensors at once and record data to an SD card.

The following sensors have been implemented thus far: 
  1. Atlas Temperature sensor
  2. Atlas Dissolved Oxygen sensor 
  3. Atlas Flow Rate sensor 
  4. Catnip Electronics Soil Moisture sensor 
  
The implementation of the sensors allow for easy removal of sensors that are unwanted, as well as addition of sensors as needed. 

Structure: 
- A virtual class that is comprised of sensors 
- All other classes represent an implemented sensor that inherits virtual class functions  
- Array of sensors classes to access the data recordings. 
