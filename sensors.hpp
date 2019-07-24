/* file:   sensors.hpp
   about:  Interface for any sensors that are added to the pod.
           Implements virtual function getData() to access sensor-data

   Chelsea Fernandes
*/

#ifndef sensors_h
#define sensors_h

using namespace std;

class Sensors
{
    // Data members of class
  public:
    // Pure Virtual Function
    virtual char* getData() = 0;

    //variables
    char tempdata[20];  ////we make a 20 byte character array to hold incoming data from a pc/mac/other.
    uint8_t temp_in_char;                //used as a 1 byte buffer to store in bound bytes from the RTD Circuit.
    uint8_t code;
    int time_;
};
#endif /* sensors_h */
