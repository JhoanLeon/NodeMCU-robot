#include <ESP8266WiFi.h>
#include <Ticker.h>

#define wifi_led D4 // pin on ESP12E board and it's negative
#define control_led D0 // pin to control through WiFi

String ssid = "CLARO-C84A";
String password = "Cl4r0@F2C84A";

WiFiServer server(80);
int status_led = 0;

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
  pinMode(control_led, OUTPUT);
  
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

    server.begin(); // starts web server
    digitalWrite(control_led, LOW); // turn on led
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
  WiFiClient client = server.available(); // search on server for a new client

  if (!client)
  {
    return; // this return restart void loop
  }

  while (!client.available()) // wait until appear an available client
  {
    delay(1);
  }

  String request = client.readStringUntil('\r'); // read client's request

  if (request.indexOf("/favicon") == -1) // if favicon doesn't apper on request
  {
    Serial.println("New client...");
    Serial.println(request); // show client's request
  }

  client.flush(); // clear client's request buffer   
  
  if (request.indexOf("/START") != -1)
  {
    status_led = 1;
  } 
  if (request.indexOf("/STOP") != -1)
  {
    status_led = 0;
  }

  digitalWrite(control_led, status_led);

  // web page
  client.println("HTTP/1.1 200 OK");
  client.println("");
  client.println("");
  client.println("");
  client.println("");

  client.println("<!DOCTYPE html><html><head><title>FPGA-ROBOT</title></head><body>");
  client.println("<h1 style='text-align: center; background-color: #0B1EF5; color: #FFFFFF'> Robot WiFi Remote Interface </h1>");
  client.println("<div style='text-align: center; margin: 10px; background-color: #0B77F5' class='input'>");
  client.println("<h2>Initial Position</h2>");
  client.println("<label for='html'>x_0:</label><input type='text' placeholder='enter initial position'><br>");
  client.println("<label for='html'>y_0:</label><input type='text' placeholder='enter initial position'>");
  client.println("<h2>Goal Position</h2>");
  client.println("<label for='html'>x_n:</label><input type='text' placeholder='enter goal position'><br>");
  
  if (status_led == 1)
    client.println("<label for='html'>led_on:</label><input type='text' placeholder='enter goal position'>");
  else
    client.println("<label for='html'>led_off:</label><input type='text' placeholder='enter goal position'>");
  
  client.println("<h2>Commands</h2>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START'><h3> Start </h3></button>");
  client.println("<button style='background-color: #F53711; border-radius: 10px; margin:5px' type='button' onClick=location.href='/STOP'><h3> Stop </h3></button>");
  client.println("</div>");
  client.println("</body></html>");
  //end of web page

  delay(1);
  
  if (request.indexOf("/favicon") == -1) 
  {
    Serial.println("Request finished");
    Serial.println("");
  }

}
