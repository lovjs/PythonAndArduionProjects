
#define LED 13     
#define IN_PORT 0     
#define NUMLOOPS 90     
#define LOOP_DELAY 1000  

//initializes/defines the output pin of the LM335 temperature sensor
int outputPin= 0;
//this sets the ground pin to LOW and the input voltage pin to high
void setup()
{
Serial.begin(9600);
Serial.println("Hello");  
pinMode(LED, OUTPUT);     
}

//main loop
void loop()
{

 char command;
  int value;
  int rawvoltage= analogRead(outputPin);
  float millivolts= (rawvoltage/1024.0) * 5000;
  float kelvin= (millivolts/10);
  float celsius= kelvin - 273.15;
  float fahrenheit= ((celsius * 9)/5 +32);
  if (Serial.available() > 0) {         
    command = (char)Serial.read();   
    if (command == 'z') {              
      Serial.println("Degrees Fahrenheit:\n");        
      for (int i = 0; i < NUMLOOPS; i++) {
        rawvoltage= analogRead(outputPin);
        millivolts= (rawvoltage/1024.0) * 5000;
        kelvin= (millivolts/10);
        celsius= kelvin - 273.15;
        fahrenheit= ((celsius * 9)/5 +32);
        delay(LOOP_DELAY);             
        Serial.println(fahrenheit);
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


delay(1000);
}
