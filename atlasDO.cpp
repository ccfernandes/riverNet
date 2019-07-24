/* file:  atlasDO.cpp
   about: Reads data from Atlas Dissolved oxygen sensor and returns to main program

   Chelsea Fernandes, Sukruth Krishnakumar
*/

#include "atlasDO.hpp"

#include <Wire.h>   //enable I2C
#include <SPI.h>
#include <SD.h>
#include "SleepyPi2.h" //library used for sleep mode
#include <TimeLib.h>
#include <LowPower.h>
#include <PCF8523.h> //RTC library

//#include <string.h>
//#include <dos.h>
//#include <cstddef>

#define address 97               //default I2C ID number for EZO D.O. Circuit.

/***************************************************************
  variable initialization
 ***************************************************************/
AtlasDO::AtlasDO() {
  strcpy(tempdata, "r"); //we make a 20 byte character array to hold incoming data from a pc/mac/other.
  code = 0;                   //used to hold the I2C response code.
  temp_in_char = 0;                //used as a 1 byte buffer to store inbound bytes from the D.O. Circuit.
  time_ = 575;                 //used to change the delay needed depending on the command sent to the EZO Class D.O. Circuit.
}

/***************************************************************
  Retrieve data from the DO sensor and return to main
 ***************************************************************/
char* AtlasDO::getData()
{
  uint8_t i = 0;                                                             //counter used for DO_data array.
  time_ = 575;

  Wire.beginTransmission(address);                                      //call the circuit by its ID number.
  Wire.write(tempdata);                                             //transmit the command that was sent through the serial port.
  Wire.endTransmission();                                               //end the I2C data transmission.

  if (strcmp(tempdata, "sleep") != 0) {                              //if the command that has been sent is NOT the sleep command, wait the correct amount of time and request data.
    //if it is the sleep command, we do nothing. Issuing a sleep command and then requesting data will wake the D.O. circuit.
    //        delay(time_);                   //wait the correct amount of time for the circuit to complete its instruction.
    delay(time_);
    Wire.requestFrom(address, 20, 1);                                     //call the circuit and request 20 bytes (this may be more than we need)
    code = Wire.read();                                                   //the first byte is the response code, we read this separately.

    switch (code) {                   //switch case based on what the response code is.
      case 1:                         //decimal 1.
        //                 Serial.println("Success");    //means the command was successful.
        break;                        //exits the switch case.

      case 2:                         //decimal 2.
        Serial.println("Failed");     //means the command has failed.
        break;                        //exits the switch case.

      case 254:                      //decimal 254.
        Serial.println("Pending");   //means the command has not yet been finished calculating.
        break;                       //exits the switch case.

      case 255:                      //decimal 255.
        Serial.println("No Data");   //means there is no further data to send.
        break;                       //exits the switch case.
    }


    while (Wire.available()) {       //are there bytes to receive.
      temp_in_char = Wire.read();         //receive a byte.
      DO_data[i] = temp_in_char;          //load this byte into our array.
      i += 1;                        //incur the counter for the array element.
      if (temp_in_char == 0) {            //if we see that we have been sent a null command.
        i = 0;                       //reset the counter i to 0.
        Wire.endTransmission();      //end the I2C data transmission.
        break;                       //exit the while loop.
      }
    }

    if (isDigit(DO_data[0])) {
      string_pars();                  //If the first char is a number we know it is a DO reading, lets parse the DO reading
    }
    else {                            //if it’s not a number
      //Serial.println(DO_data);        //print the data.
      //            logfile.print(", ");
      //            logfile.print(DO_data);
      for (i = 0; i < 20; i++) {      //step through each char
        DO_data[i] = 0;               //set each one to 0 this clears the memory
      }
    }
  }
  //    strcpy(tempdata, "sleep");
  char sleepdata[] = "sleep";
  Wire.beginTransmission(address);
  Wire.write(sleepdata);
  Wire.endTransmission();
  //    delay(10000);
  return DO_data;
}

char* AtlasDO::string_pars() {                  //this function will break up the CSV string into its 2 individual parts, DO and %sat.
  uint8_t flag = 0;                      //this is used to indicate is a “,” was found in the string array
  uint8_t i = 0;                         //counter used for DO_data array.


  for (i = 0; i < 20; i++) {          //Step through each char
    if (DO_data[i] == ',') {          //do we see a ','
      flag = 1;                       //if so we set the var flag to 1 by doing this we can identify if the string being sent from the DO circuit is a CSV string containing tow values
    }
  }

  return DO_data;
  /*                                //uncomment this section if you want to take the ASCII values and convert them into a floating point number.
    DO_float=atof(DO);
    sat_float=atof(sat);
  */
}
