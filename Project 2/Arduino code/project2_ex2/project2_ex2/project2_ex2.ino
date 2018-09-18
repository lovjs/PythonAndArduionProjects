int flag = 0;        // set when data is collected
const int N = 150;   // how many data points?

int v1[N];// raw analogRead vals
int v2[N];
long times[N];       // times (microseconds)

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(3,OUTPUT);
  Serial.begin(9600);
  analogWrite(3,51);
}

/*
** collectData iterates through the arrays and stores
** times and raw voltages as quickly as possible
*/

void collectData() {
  for (int i=0; i<N; i++) {
    v1[i]=analogRead(0);
    v2[i]=analogRead(1);
    times[i]=micros();
  }
}

/*
** printData iterates through the arrays and prints the
** stored values back to the computer.
*/

void printData() {
  Serial.print("Time:");
  Serial.print("\tVoltage 1:");
  Serial.println("\tVoltage 2:");
  for (int i=0; i<N; i++) {
    Serial.print(times[i]);
    Serial.print("\t");
    Serial.print(v1[i] * (5.0 / 1023.0));
    Serial.print("\t\t");
    Serial.println(v2[i] * (5.0 / 1023.0));
  }
}

void loop()
{
  if (flag==0) {
    collectData();
    printData();
    flag=1;
  } else {
    /*
    ** We got the data, just flash the LED now.
    */
    digitalWrite(13, HIGH);
    delay(1000); // Wait for 1000 millisecond(s)
    digitalWrite(13, LOW);
    delay(1000); // Wait for 1000 millisecond(s)
  }
}
