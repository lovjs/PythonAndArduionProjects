
#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#define LED 13     
#define IN_PORT 0     
#define NUMLOOPS 20     
#define LOOP_DELAY 1000  
//////////////////////////
// MicroOLED Definition //
//////////////////////////
//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 9  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 1 

//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration
int outputPin= 0;
void setup()
{
  delay(100);
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
  Serial.begin(9600);
Serial.println("Hello");  
pinMode(LED, OUTPUT); 
}



void loop()
{
  char command;
  int rawvoltage= analogRead(outputPin);
  float millivolts= (rawvoltage/1024.0) * 5000;
  float kelvin= (millivolts/10);
  float celsius= kelvin - 273.15;
  float fahrenheit= ((celsius * 9)/5 +32);
  if (Serial.available() > 0) {         
    command = (char)Serial.read();   
    if (command == 'z') {                   
      for (int i = 0; i < NUMLOOPS; i++) {
        delay(LOOP_DELAY);             
        oled.clear(PAGE);            // Clear the display
    oled.setCursor(0, 0);        // Set cursor to top-left
    oled.setFontType(0);         // Smallest font
    oled.print("Degrees: \n");          // Print "A0"
    oled.setFontType(2);         // 7-segment font
    oled.print(fahrenheit);  // Print a0 reading
    oled.setCursor(0, 16);       // Set cursor to top-middle-left
    oled.setFontType(0);         // Repeat
    oled.display();
      }
      Serial.println("End");     
    }
  }
  else {
    digitalWrite(LED, HIGH);       
    delay(LOOP_DELAY);
    digitalWrite(LED, LOW);
    delay(LOOP_DELAY);
  } 
  
}


