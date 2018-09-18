int powerValue = 0; // declares the value for power for the write
void setup() { 
  pinMode(3,OUTPUT); //activates pin 3 as output
  Serial.begin(9600); // sets the bit rate
  Serial.print("Voltage1:"); // Creates column header for voltage1 
  Serial.println("\tVoltage2:"); // Creates column header for voltage2
}

void loop() { 
  if(powerValue < 250){
  analogWrite(3, powerValue); // gives the power to the board
  delay(500); // will wait .5 second before excuteing loop again
  float voltage1 = analogRead(2) * (5.0 / 1023.0); // creates variable of float type and calculates voltage
  float voltage2 = analogRead(3) * (5.0 / 1023.0); // creates variable of float type and calculates voltage
  Serial.print(voltage1); // prints the value of voltage1 to the serial monitor
  Serial.print("\t  \t"); //tabs in order to make nice columns
  Serial.print(voltage2); // prints the value of voltage2 to the serial monitor
  Serial.print("\t \t \t"); //tab for aethstetics
  Serial.println(powerValue); //print power value
  powerValue += 5;  // increments the power
  }
}
