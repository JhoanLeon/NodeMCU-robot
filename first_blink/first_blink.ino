
// led on ESP12E (blue pilot) is attach to D4 (GPIO2)
// led on board (pilot near to USB connector, it is connected pull up) is attach to D0 (GPIO16)

#define my_led D0 // pins are indicated by both GPIOX or DX 
#define pi_led D4 

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(my_led, OUTPUT);
  pinMode(pi_led, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(my_led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(pi_led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(my_led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  digitalWrite(pi_led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
