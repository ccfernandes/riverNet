/* file:   atlasDO.hpp
   about:  Reads data from Atlas Dissolved oxygen sensor and
           will write to SD card in the (.ino) file

   Chelsea Fernandes
*/

#ifndef atlasDO_hpp
#define atlasDO_hpp

#include <stdio.h>
#include "sensors.hpp"

class AtlasDO: public Sensors
{
  public:
    AtlasDO();  //initialization
    char* getData();             //retrieve data from sensor as a string
    char* string_pars();
  private:
    char DO_data[20];                //we make a 20 byte character array to hold incoming data from the D.O. circuit.
    char *DO;                        //char pointer used in string parsing.
    char *sat;                       //char pointer used in string parsing.
};
#endif /* atlasDO_hpp */
