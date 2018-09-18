int powerValue = 0; // declares the value for power for the write
void setup() { 
  pinMode(3,OUTPUT); //activates pin 3 as output
  Serial.begin(9600); // sets the bit rate
  Serial.print("Befr_R2:"); // Creates column header for voltage1 
  Serial.print("\tAftr_R2:"); // Creates column header for voltage2
  Serial.print("\t\tBefr_R3: ");
  Serial.println("\t\tPower Value: ");

} 
void loop() { 
  if(powerValue < 250){
  analogWrite(3, powerValue); // gives the power to the board
  delay(500); // will wait .5 second before excuteing loop again
  float befr_R2 = analogRead(0) * (5.0 / 1023.0); // creates variable of float type and calculates voltage
  float aftr_R2 = analogRead(1) * (5.0 / 1023.0); // creates variable of float type and calculates voltage
  float befr_R3 = analogRead(2) * (5.0 / 1023.0); // creates variable of float type and calculates voltage
  Serial.print(befr_R2); // prints the value of voltage1 to the serial monitor
  Serial.print("\t  \t"); //tabs in order to make nice columns
  Serial.print(aftr_R2); // prints the value of voltage2 to the serial monitor
  Serial.print("\t \t \t"); //tab for aethstetics
  Serial.print(befr_R3); // prints the value of voltage2 to the serial monitor
  Serial.print("\t \t \t"); //tab for aethstetics
  Serial.println(powerValue); //print power value
  powerValue += 1;  // increments the power
  }
}
