#include <ESP8266WiFi.h>
#include <Ticker.h>

#define wifi_led D4 // pin on ESP12E board and it's negative

String ssid = "CLARO-C84A";
String password = "Cl4r0@F2C84A";

Ticker tic_wifi_led;

byte cont = 0;
byte max_int = 50;


void blink_led()
{
  byte state = digitalRead(wifi_led);
  digitalWrite(wifi_led, !state);
}


void setup() 
{
  pinMode(wifi_led, OUTPUT);
  // Begin serial port with PC
  Serial.begin(115200);
  Serial.println("\n");

  tic_wifi_led.attach(0.2, blink_led); // blink led every 0.2 seconds (200ms)

  // WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and cont < max_int) // wait until connection or max attempts (50)
  {
    Serial.print(".");
    delay(500);
    cont = cont + 1;
  }

  tic_wifi_led.detach();
  Serial.println("\n");

  if (cont < max_int) // connection successfull
  {
    digitalWrite(wifi_led, HIGH); // turn off wifi led
    Serial.println("*****************************************");
    Serial.print("Connected to WiFi: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC ADRESS: ");
    Serial.println(WiFi.macAddress());
    Serial.println("*****************************************");
  }
  else // no connection
  {
    digitalWrite(wifi_led, LOW); // turn on wifi led
    Serial.println("-----------------------------------------");
    Serial.println("Connection error");
    Serial.println("-----------------------------------------");  
  }
  
}


void loop() 
{
  //fucker
}
