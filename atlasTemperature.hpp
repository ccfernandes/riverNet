/* file:   atlasTemperature.hpp
   about:  Reads data from atlas temperature probe and
           will write to SD card in the (.ino) file

   Chelsea Fernandes
*/

#ifndef atlasTemperature_hpp
#define atlasTemperature_hpp

#include <stdio.h>
#include "sensors.hpp"

class AtlasTemperature: public Sensors
{
  public:
    AtlasTemperature();  //initialization
    char* getData();             //retrieve data from sensor as a string
    char* getTemp() {
      return RTD_data;
    }
  public:
    //variables
    char RTD_data[20];               //we make a 20 byte character array to hold incoming data from the RTD circuit.
    uint8_t i;                     //counter used for RTD_data array.
    float tmp_float;                 //float var used to hold the float value of the RTD.
};
#endif /* atlasTemperature_hpp */
