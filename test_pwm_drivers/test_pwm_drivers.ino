int pwm1 = 5;
int pwm2 = 6;

void setup() 
{
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  
}

void loop() 
{
  analogWrite(pwm1, 255);
  analogWrite(pwm2, 255);
}
