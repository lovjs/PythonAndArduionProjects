#include <Sweep.h>
#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#define LED 13     
#define IN_PORT 0     
#define NUMLOOPS 20     
#define LOOP_DELAY 1000  

//#include <SoftwareSerial.h>
#define PIN_RESET 9  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 1 

MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration
int outputPin= 0;
//SoftwareSerial Serial1(2, 1);
// Create a Sweep device using Serial #1 (RX1 & TX1)
Sweep device(Serial1);
// keeps track of how many scans have been collected
uint8_t scanCount = 0;
// keeps track of how many samples have been collected
uint16_t sampleCount = 0;

// Arrays to store attributes of collected scans
bool syncValues[500];         // 1 -> first reading of new scan, 0 otherwise
float angles[500];            // in degrees (accurate to the millidegree)
uint16_t distances[500];      // in cm
uint8_t signalStrengths[500]; // 0:255, higher is better

// Finite States for the program sequence
const uint8_t STATE_WAIT_FOR_USER_INPUT = 0;
const uint8_t STATE_ADJUST_DEVICE_SETTINGS = 1;
const uint8_t STATE_VERIFY_CURRENT_DEVICE_SETTINGS = 2;
const uint8_t STATE_BEGIN_DATA_ACQUISITION = 3;
const uint8_t STATE_GATHER_DATA = 4;
const uint8_t STATE_STOP_DATA_ACQUISITION = 5;
const uint8_t STATE_REPORT_COLLECTED_DATA = 6;
const uint8_t STATE_RESET = 7;
const uint8_t STATE_ERROR = 8;

// Current state in the program sequence
uint8_t currentState = STATE_WAIT_FOR_USER_INPUT;

// String to collect user input over serial
String userInput = "";

void setup()
{
  // Initialize serial
  delay(100);
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer. 
  pinMode(LED, OUTPUT); 
  Serial.begin(9600);    // serial terminal on the computer
  Serial1.begin(115200); // sweep device
  Serial.println("Hello"); 

  // reserve space to accumulate user message
  userInput.reserve(50);

  // initialize counter variables and reset the current state
  reset();
}

// Loop functions as an FSM (finite state machine)
void loop()
{
  switch (currentState)
  {
  case STATE_WAIT_FOR_USER_INPUT:
    if (listenForUserInput())
      currentState = STATE_ADJUST_DEVICE_SETTINGS;
    break;
  case STATE_ADJUST_DEVICE_SETTINGS:
    currentState = adjustDeviceSettings() ? STATE_VERIFY_CURRENT_DEVICE_SETTINGS : STATE_ERROR;
    break;
  case STATE_VERIFY_CURRENT_DEVICE_SETTINGS:
    currentState = verifyCurrentDeviceSettings() ? STATE_BEGIN_DATA_ACQUISITION : STATE_ERROR;
    break;
  case STATE_BEGIN_DATA_ACQUISITION:
    currentState = beginDataCollectionPhase() ? STATE_GATHER_DATA : STATE_ERROR;
    break;
  case STATE_GATHER_DATA:
    gatherSensorReading();
    if (scanCount > 3)
      currentState = STATE_STOP_DATA_ACQUISITION;
    break;
  case STATE_STOP_DATA_ACQUISITION:
    currentState = stopDataCollectionPhase() ? STATE_REPORT_COLLECTED_DATA : STATE_ERROR;
    break;
  case STATE_REPORT_COLLECTED_DATA:
    printCollectedData();
    bool flag;
    flag = false;
      for(int i = 0; i < 150; i ++){
        Serial.println("Angle: " + String(angles[i], 3) + ", Distance: " + String(distances[i]) +  "\n" );
        if(((angles[i] > 350) || (angles[i]< 10)) && (distances[i] < 25)){  
          flag = true;
        }
      }
      
        if(flag){
         oled.clear(PAGE);            // Clear the display
         oled.setCursor(0, 0);        // Set cursor to top-left
         oled.setFontType(0);         // Smallest font
         oled.print("Hi Steve\n Please\n give\n us an\n A!");         
         oled.display();
        }else{
        oled.clear(PAGE);            // Clear the display
        oled.setCursor(0, 0);        // Set cursor to top-left
        oled.setFontType(0);         // Smallest font
        oled.print("No Steve\n");         
        oled.display();
        }
      
    currentState = STATE_RESET;
    break;
  case STATE_RESET:
    Serial.println("\n\nAttempting to reset and run the program again...");
    reset();
    currentState = STATE_WAIT_FOR_USER_INPUT;
    break;
  default: // there was some error
    Serial.println("\n\nAn error occured. Attempting to reset and run program again...");
    reset();
    currentState = STATE_WAIT_FOR_USER_INPUT;
    break;
  }
  for(int i = 0; i < 360; i++){
    if(angles[i] > 350 && angles[i] < 500){
       
    }
  }
}

// checks if the user has communicated anything over serial
// looks for the user to send "start"
bool listenForUserInput()
{
  while (Serial.available())
  {
    userInput += (char)Serial.read();
  }
  if (userInput.indexOf("start") != -1)
  {
    Serial.println("Registered user start.");
    return true;
  }
  return false;
}

// Adjusts the device settings
bool adjustDeviceSettings()
{
  // Set the motor speed to 5HZ (codes available from 1->10 HZ)
  bool bSuccess = device.setMotorSpeed(MOTOR_SPEED_CODE_5_HZ);
  Serial.println(bSuccess ? "\nSuccessfully set motor speed." : "\nFailed to set motor speed");

  /*  
  // Device will always default to 500HZ scan rate when it is powered on.
  // Snippet below is left for reference.
  // Set the sample rate to 500HZ (codes available for 500, 750 and 1000 HZ)
  bool bSuccess = device.setSampleRate(SAMPLE_RATE_CODE_500_HZ);
  Serial.println(bSuccess ? "\nSuccessfully set sample rate." : "\nFailed to set sample rate.");
*/
  return bSuccess;
}

// Querries the current device settings (motor speed and sample rate)
// and prints them to the console
bool verifyCurrentDeviceSettings()
{
  // Read the current motor speed and sample rate
  int32_t currentMotorSpeed = device.getMotorSpeed();
  if (currentMotorSpeed < 0)
  {
    Serial.println("\nFailed to get current motor speed");
    return false;
  }
  int32_t currentSampleRate = device.getSampleRate();
  if (currentSampleRate < 0)
  {
    Serial.println("\nFailed to get current sample rate");
    return false;
  }

  // Report the motor speed and sample rate to the computer terminal
  Serial.println("\nMotor Speed Setting: " + String(currentMotorSpeed) + " HZ");
  Serial.println("Sample Rate Setting: " + String(currentSampleRate) + " HZ");

  return true;
}

// Initiates the data collection phase (begins scanning)
bool beginDataCollectionPhase()
{
  // Attempt to start scanning
  Serial.println("\nWaiting for motor speed to stabilize and calibration routine to complete...");
  bool bSuccess = device.startScanning();
  Serial.println(bSuccess ? "\nSuccessfully initiated scanning..." : "\nFailed to start scanning.");
  if (bSuccess)
    Serial.println("\nGathering 3 scans...");
  return bSuccess;
}

// Gathers individual sensor readings until 3 complete scans have been collected
void gatherSensorReading()
{
  // attempt to get the next scan packet
  // Note: getReading() will write values into the "reading" variable
  bool success = false;
  ScanPacket reading = device.getReading(success);
  if (success)
  {
    // check if this reading was the very first reading of a new 360 degree scan
    if (reading.isSync())
      scanCount++;

    // don't collect more than 3 scans
    if (scanCount > 3)
      return;

    // store the info for this sample
    syncValues[sampleCount] = reading.isSync();
    angles[sampleCount] = reading.getAngleDegrees();
    distances[sampleCount] = reading.getDistanceCentimeters();
    signalStrengths[sampleCount] = reading.getSignalStrength();

    // increment sample count
    sampleCount++;
  }
}

// Terminates the data collection phase (stops scanning)
bool stopDataCollectionPhase()
{
  // Attempt to stop scanning
  bool bSuccess = device.stopScanning();

  Serial.println(bSuccess ? "\nSuccessfully stopped scanning." : "\nFailed to stop scanning.");
  return bSuccess;
}

// Prints the collected data to the console
// (only prints the complete scans, ignores the first partial)
void printCollectedData()
{
  Serial.println("\nPrinting info for the collected scans (NOT REAL-TIME):");

  int indexOfFirstSyncReading = 0;
  // don't print the trailing readings from the first partial scan
  while (!syncValues[indexOfFirstSyncReading])
  {
    indexOfFirstSyncReading++;
  }
  // print the readings for all the complete scans
  for (int i = indexOfFirstSyncReading; i < sampleCount; i++)
  {
    if (syncValues[i])
    {
      Serial.println("\n----------------------NEW SCAN----------------------");
    }
    Serial.println("Angle: " + String(angles[i], 3) + ", Distance: " + String(distances[i]) + ", Signal Strength: " + String(signalStrengths[i]));
  }
}

// Resets the variables and state so the sequence can be repeated
void reset()
{
  scanCount = 0;
  sampleCount = 0;
  // reset the sensor
  device.reset();
  delay(50);
  Serial.flush();
  userInput = "";
  Serial.println("\n\nWhenever you are ready, type \"start\" to to begin the sequence...");
  currentState = 0;
}
