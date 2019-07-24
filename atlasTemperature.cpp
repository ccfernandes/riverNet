/* file:   atlasTemperature.cpp
   about:  Reads data from atlas temperature probe and
           will write to SD card in the (.ino) file

   Chelsea Fernandes, Sukruth Krishnakumar
*/

#include "atlasTemperature.hpp"
#include <Wire.h>   //enable I2C
#include <SPI.h>
#include <SD.h>
#include "SleepyPi2.h" //library used for sleep mode
#include <TimeLib.h>
#include <LowPower.h>
#include <PCF8523.h> //RTC library

//#include <cstddef>
//#include <string.h>
//#include <dos.h>
//#include <time.h>

#define tempAddress 102          //default I2C ID number for EZO RTD Circuit.(temperature)

/****************************************************************
  initialize all variables here
 ****************************************************************/
AtlasTemperature::AtlasTemperature() {
  //    tempdata[20] = "sleep";     //we make a 20 byte character array to hold incoming data from a pc/mac/other.
  // strcpy(tempdata, "r");
  temp_in_char = 0;             //used as a 1 byte buffer to store in bound bytes from the RTD Circuit.
  i = 0;                        //counter used for RTD_data array.
  code = 0;                     //used to hold the I2C response code.
  time_ = 600;
}

/****************************************************************
  retrieve data from temperature sensor and return to main
 ****************************************************************/
char* AtlasTemperature::getData() {
  strcpy(tempdata, "r");
  //for (i = 0; i <= received_temp_from_comp; i++) {
  //set all char to lower case, this is just so this exact sample code can recognize the "sleep" command.
  //tempdata[i] = tolower(tempdata[i]);
  //"Sleep" ≠ "sleep"
  //}
  i = 0;   //reset i, we will need it later
  //if a command has been sent to calibrate or take a reading we wait 600ms so that the circuit has time to take the reading.
  if (tempdata[0] == 'c' || tempdata[0] == 'r') time_ = 600;
  //    else time_ = 250;
  //if any other command has been sent we wait only 250ms.

  Wire.beginTransmission(tempAddress);    //call the circuit by its ID number.
  Wire.write(tempdata);                   //transmit the command that was sent through the serial port.
  Wire.endTransmission();                 //end the I2C data transmission.

  //if the command that has been sent is NOT the sleep command, wait the correct amount of time and request data.
  //if it is the sleep command, we do nothing. Issuing a sleep command and then requesting data will wake the RTD circuit.
  if (strcmp(tempdata, "sleep") != 0) {
    delay(time_);                         //wait the correct amount of time for the circuit to complete its instruction.

    Wire.requestFrom(tempAddress, 20, 1);
    //call the circuit and request 20 bytes (this may be more than we need)
    code = Wire.read();                     //the first byte is the response code, we read this separately.

    switch (code) {                         //switch case based on what the response code is.
      case 1:                             //decimal 1.
        //                Serial.print("Success");      //means the command was successful
        break;                          //exits the switch case.

      case 2:                             //decimal 2.
        //                Serial.println("Failed");     //means the command has failed.
        break;                          //exits the switch case.

      case 254:                           //decimal 254.
        //                Serial.println("Pending");        //means the command has not yet been finished calculating.
        break;                            //exits the switch case.

      case 255:                           //decimal 255.
        //                Serial.println("No Data");        //means there is no further data to send.
        break;                            //exits the switch case.

    }
    while (Wire.available()) {            //are there bytes to receive.
      temp_in_char = Wire.read();       //receive a byte.
      RTD_data[i] = temp_in_char;       //load this byte into our array.
      i += 1;                           //incur the counter for the array element.
      if (temp_in_char == 0) {          //if we see that we have been sent a null command.
        i = 0;                        //reset the counter i to 0.
        Wire.endTransmission();       //end the I2C data transmission.
        break;                        //exit the while loop.
      }
    }
    char sleepdata[] = "sleep";
    Wire.beginTransmission(tempAddress);
    Wire.write(sleepdata);
    Wire.endTransmission();
    //    delay(10000);

    /***if we want to print the data in the sensor files, put it here**/
    return RTD_data;
  }

  //Uncomment this section if you want to take the RTD value and convert it into floating point number.
  //  RTD_float=atof(RTD_data);
}
