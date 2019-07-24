/*
   file:  newRivernet.ino
   about: -This program writes data from sensors (.cpp files) to SD card.
          Incorporates sleep mode to read/write data at a given interval.
          -To add a sensor, include its header and cpp file and
          a couple lines of code to print data in the "printSensors" function
          -Note: the sensors are connected and coded to support I2C, not UART

   Chelsea Fernandes, Sukruth Krishnakumar
*/

/*
  works with pro mini pretty well
  current:  reading data = 56mA, sleep = 9.5mA
   ^ tested using a DC power supply 
  July 18, 2019
*/

/**************built in libraries***************/
#include <SPI.h>
#include <SD.h>
#include <SleepyPi2.h> //library used for sleep mode 
#include <TimeLib.h>
#include <LowPower.h>
#include <PCF8523.h> //RTC library 
#include <Wire.h>   //enable I2C 
#include "Vector.h"

/****************local files*******************/
#include "atlasTemperature.hpp"
#include "atlasDO.hpp"
#include "atlasFlow.hpp"
#include "sensors.hpp"
/****************the logger variables*****************/
// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL  3000 // mills between entries (reduce to take more/faster data) = intervals between serial monitor data entries 
#define MAX_FILE_SIZE 10000 // start new file only when previous reached 100 KB 
#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

// The analog pins that connect to the sensors
//#define tempPin 1                // analog 0

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

//const char *monthName[12] = {
//  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
//  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
//};

// the logging and error files
File logfile;
File errors;

/*****************CHANGE DATA LOGGING FREQUENCY/INTERVAL HERE*****************/

//Note: there is currently an additional 3 second delay to the given interval
// .. use either eTB_SECOND or eTB_MINUTE or eTB_HOUR
eTIMER_TIMEBASE  PeriodicTimer_Timebase     = eTB_SECOND;   // e.g. Timebase set to seconds. Other options: eTB_MINUTE, eTB_HOUR
uint8_t          PeriodicTimer_Value        = 20;           // Timer Interval in units of Timebase e.g 10 seconds = 10

/*****************other variables*******************/

PCF8523 RTC; // define the Real Time Clock object
typedef Sensors* sensorPtr; //to print the sensor data
sensorPtr arr[5];

/********************functions****************************/

void setup() {
  Serial.begin(9600);
  Serial.println("HEY");

  Serial.println();
  Wire.begin();            //enable I2C port
  pinMode(8, OUTPUT);
  pinMode(10, OUTPUT); //  make sure that the default chip select pin is set to output, even if you don't use it:
  digitalWrite(8, HIGH);

#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START
  initializeSD();
  initializeRTC();
  openError();
  openFile();

  logfile.println("millis,stamp,datetime,temp (F),DO,Flow Rate");
#if ECHO_TO_SERIAL
  Serial.println("millis,stamp,datetime,temp(F),DO,Flow Rate");
#endif //ECHO_TO_SERIAL
  //  delay(500);

  //sensor object array initialization
  arr[0] = new AtlasTemperature();
  arr[1] = new AtlasDO();
  arr[2] = new AtlasFlow();

  pinMode(2, INPUT_PULLUP); //interrupt pin for wakeup
  SleepyPi.rtcInit(true);

  Serial.println("Setup complete");


  //first iteration of data collection to reduce high consumption for first minute
  timestamp(logfile);
  printSensors();
  logfile.println();
  Serial.print("\n");
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  logfile.flush();
}

void loop() {
  digitalWrite(8, LOW);
  sleepMode(); // enter/exit sleep mode to minimize power consumption
  digitalWrite(8, HIGH);

  if (logfile.size() >= MAX_FILE_SIZE) {
    logfile.close();
    openFile();
  }

  timestamp(logfile);

  printSensors(); //prints the sensor data. add to this function below

  logfile.println();
  Serial.print("\n");

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();

  logfile.flush();
}

//enters and exits sleep mode at a given interval (defined at beginning of file)
void sleepMode() {
  SleepyPi.rtcClearInterrupts(); //clear interrupt flags

  // Allow wake up alarm to trigger interrupt on falling edge.
  attachInterrupt(0, alarm_isr, FALLING);    // Alarm pin

  SleepyPi.setTimer1(PeriodicTimer_Timebase, PeriodicTimer_Value); // Set the Periodic Timer

  // Enter power down state with ADC and BOD module disabled.
  // Wake up when wake up pin is low.
  SleepyPi.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  // Disable external pin interrupt on wake up pin.
    detachInterrupt(0);

  SleepyPi.ackTimer1();
}

void printSensors()
{
  //print to Serial
  Serial.print(", ");
  Serial.print(arr[0]->getData());
  Serial.print(", ");
  Serial.print(arr[1]->getData());
  Serial.print(", ");
  Serial.print(arr[2]->getData());
  //...add more sensors here for Serial

  //print to SD card
  logfile.print(", ");
  logfile.print(arr[0]->getData());
  logfile.print(", ");
  logfile.print(arr[1]->getData());
  logfile.print(", ");
  logfile.print(arr[2]->getData());
  //...add more sensors here for SD 
}

//Initializes SD card
void initializeSD() {
  // initialize the SD card
#if ECHO_TO_SERIAL
  Serial.println("Initializing SD card...");
#endif //ECHO_TO_SERIAL  

  if (!SD.begin(chipSelect)) {   } // see if the card is present and can be initialized:
#if ECHO_TO_SERIAL
  Serial.println("card initialized.");
#endif //ECHO_TO_SERIAL
}

//initialize the real time clock
void initializeRTC() {
  Wire.begin();
  while (!RTC.begin()) {
    errors.print("RTC failed on line ");
    errors.println(__LINE__);
    delay(500);                 // tries again every 0.5 seconds until it works
  }
}

// for interrupt (sleep mode), nothing goes here
void alarm_isr() {
}

//Opens the correct file to write to and creates a new file if necessary
void openFile() {
  // create a new file
  char filename[] = "logger00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    unsigned long filesize = 0;
    logfile = SD.open(filename, FILE_WRITE);
    if (logfile.size() < MAX_FILE_SIZE) {
      break;
    }
    else {
      logfile.close();
    }
  }
  if (! logfile) {
    //error("couldn't create file",__LINE__);  // should I include more possible files (1000) or just let it stop when it runs out?
  }
#if ECHO_TO_SERIAL
  Serial.print("Logging to: ");
  Serial.println(filename);
#endif //ECHO_TO_SERIAL
}

//Prints out the date and time
void timestamp(File file) {
  DateTime now;

  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

  // log milliseconds since starting
  uint32_t m = millis();
  //file.print(m);           // milliseconds since start
  file.print(", ");
#if ECHO_TO_SERIAL
  Serial.print(m);         // milliseconds since start
  Serial.print(", ");
#endif

  // fetch the time
  now = SleepyPi.readTime();
  //  Serial.print("readTime ");
  // log time
  file.print(now.unixtime()); // seconds since 1/1/1970
  file.print(", ");
  file.print('"');
  file.print(now.year(), DEC);
  file.print("/");
  file.print(now.month(), DEC);
  file.print("/");
  file.print(now.day(), DEC);
  file.print(" ");
  file.print(now.hour(), DEC);
  file.print(":");
  file.print(now.minute(), DEC);
  file.print(":");
  file.print(now.second(), DEC);
  file.print('"');
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime()); // seconds since 1/1/1970
  Serial.print(", ");
  Serial.print('"');
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print('"');
#endif //ECHO_TO_SERIAL
}

/* For the error file
    prints out the error to both the serial monitor (if ECHO_TO_SERIAL) and to the error file
    includes the line number of the call to error() for when more error statements are added to code
*/
void error(char *str, int line)
{
  timestamp(errors);      // will update to error file
#if ECHO_TO_SERIAL
  Serial.print("\n");
  Serial.print("On line ");
  Serial.print(line);
  Serial.print(" in the Arduino code, the error is:\n\t");
  Serial.println(str);
#endif //ECHO_TO_SERIAL
  errors.println();
  errors.print("On line ");
  errors.print(line);
  errors.println(" in the Arduino code, the error is:\n\t");
  errors.print("\t");
  errors.println(str);
  errors.println();
  errors.flush();
  while (1);
}

//open error file in log
void openError() {
  char filename[] = "error.txt"; // create a new file
  errors = SD.open("errors.txt", FILE_WRITE);
  errors.println();
  errors.println("Beginning a new error log");
  //  Serial.print("bef initial time ");
  timestamp(errors);
  errors.println();
#if ECHO_TO_SERIAL
  Serial.println("\nOpened error file");
#endif //ECHO_TO_SERIAL
}
