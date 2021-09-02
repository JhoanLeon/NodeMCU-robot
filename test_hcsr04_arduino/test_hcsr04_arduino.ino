#define trigger 10
#define echo 11

long time1 = 0;
long time2 = 0;
int total_time = 0;
int distance = 0; 

void setup() 
{
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);
}

void loop() 
{
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  total_time = pulseIn(echo, HIGH); // time in High on microseconds

//  while(digitalRead(echo) == 0)
//  {
//    // do nothing
//  }
//
//  time1 = millis();
//
//  while(digitalRead(echo) == 1)
//  {
//    // do nothing
//  }
//
//  time2 = millis();
//
//  total_time = time2 - time1;

  Serial.print(total_time);
  Serial.println(" us");

  //distance = (total_time/1000000)*(340/2); // (us/1000000) * (m/s) / 2
  distance = (total_time/2)/29; // in centimeters

  Serial.print(distance);
  Serial.println(" cm");

  Serial.println("");

  delay(400);
}
