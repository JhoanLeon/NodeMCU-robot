#include <ESP8266WiFi.h>
#include <Ticker.h>

#define wifi_led D4    // pin on ESP12E board and it's negative

#define led_stop        D0 // pin to control through WiFi
#define led_waypoint1   D1 // pin to control through WiFi
#define led_waypoint2   D2 // pin to control through WiFi
#define led_waypoint3   D3 // pin to control through WiFi
#define led_waypoint4   D4 // pin to control through WiFi
#define led_waypoint5   D5 // pin to control through WiFi
#define led_waypoint6   D6 // pin to control through WiFi
#define led_waypoint7   D7 // pin to control through WiFi
#define led_waypoint8   D8 // pin to control through WiFi

String ssid = "CLARO-C84A";
String password = "Cl4r0@F2C84A";

byte command = 0; // byte to code nine commands (8 waypoints and 1 stop) 0 = stop; 1 = waypoint1; 2 = waypoint2; ... 8 = waypoint8;

WiFiServer server(80);

Ticker tic_wifi_led;

// variables to connect WiFi
byte cont = 0;
byte max_int = 50;


void blink_led() // to indicate while connecting WiFi
{
  byte state = digitalRead(wifi_led);
  digitalWrite(wifi_led, !state);
}


void send_command(byte current_command) // to send through SPI command from WebPage
{
  // turn off all leds
  digitalWrite(led_stop, LOW);
  digitalWrite(led_waypoint1, LOW);
  digitalWrite(led_waypoint2, LOW);
  digitalWrite(led_waypoint3, LOW);
  digitalWrite(led_waypoint4, LOW);
  digitalWrite(led_waypoint5, LOW);
  digitalWrite(led_waypoint6, LOW);
  digitalWrite(led_waypoint7, LOW);
  digitalWrite(led_waypoint8, LOW); 

  // turn on current command on leds
  switch (current_command) 
  {
    case 0:
      digitalWrite(led_stop, HIGH);
    break; 
    
    case 1:
      digitalWrite(led_waypoint1, HIGH);
    break;
    
    case 2:
      digitalWrite(led_waypoint2, HIGH);
    break;

    case 3:
      digitalWrite(led_waypoint3, HIGH);
    break;

    case 4:
      digitalWrite(led_waypoint4, HIGH);
    break;

    case 5:
      digitalWrite(led_waypoint5, HIGH);
    break;

    case 6:
      digitalWrite(led_waypoint6, HIGH);
    break;

    case 7:
      digitalWrite(led_waypoint7, HIGH);
    break;

    case 8:
      digitalWrite(led_waypoint8, HIGH);
    break;
    
    default:
      digitalWrite(led_stop, HIGH);
    break;
  }

  // send through SPI the current command
}


void read_command() // to read commands from SPI and publish values on WebPage
{
}


void setup() 
{
  pinMode(wifi_led, OUTPUT);
  pinMode(led_stop, OUTPUT);
  pinMode(led_waypoint1, OUTPUT);
  pinMode(led_waypoint2, OUTPUT);
  pinMode(led_waypoint3, OUTPUT);
  pinMode(led_waypoint4, OUTPUT);
  pinMode(led_waypoint5, OUTPUT);
  pinMode(led_waypoint6, OUTPUT);
  pinMode(led_waypoint7, OUTPUT);
  pinMode(led_waypoint8, OUTPUT);
  
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
    digitalWrite(led_stop, HIGH); // turn on stop led
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

  if (request.indexOf("/STOP") != -1)
  {
    command = 0;
  }
  if (request.indexOf("/START1") != -1)
  {
    command = 1;
  } 
  if (request.indexOf("/START2") != -1)
  {
    command = 2;
  }
  if (request.indexOf("/START3") != -1)
  {
    command = 3;
  }
  if (request.indexOf("/START4") != -1)
  {
    command = 4;
  }
  if (request.indexOf("/START5") != -1)
  {
    command = 5;
  }
  if (request.indexOf("/START6") != -1)
  {
    command = 6;
  }
  if (request.indexOf("/START7") != -1)
  {
    command = 7;
  }
  if (request.indexOf("/START8") != -1)
  {
    command = 8;
  }
  
  send_command(command); // send command through SPI
  read_command(); // read commands from SPI

  // web page
  client.println("HTTP/1.1 200 OK");
  client.println("");
  client.println("");

  client.println("<!DOCTYPE html><html><head><title>FPGA-ROBOT</title></head><body style='font-size: 15pt'>");
  client.println("<h1 style='text-align: center; margin: 10px; background-color: #0B1EF5; color: #FFFFFF'> Mobile Robot WiFi Remote Interface </h1>");
  client.println("<div style='text-align: center; margin: 10px; padding-bottom: 15px; background-color: #0B77F5' class='input'>");
  client.println("<h2>Waypoints</h2>");
  
  client.println("<div style='column-count: 8; column-rule-style: solid; column-rule-width: 1px; margin-bottom: 10px'>");
  
  client.println("<label>x_n: 0.0m </label><br><label>y_n: 0.0m </label><br><label>theta_n: 90.0deg </label><br>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START1'><h3> Start 1 </h3></button><br>");
  
  client.println("<label>x_n: 0.0m </label><br><label>y_n: 1.0m </label><br><label>theta_n: 90.0deg </label><br>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START2'><h3> Start 2 </h3></button><br>");
  
  client.println("<label>x_n: 1.0m </label><br><label>y_n: 0.0m </label><br><label>theta_n: 90.0deg </label><br>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START3'><h3> Start 3 </h3></button><br>");
  
  client.println("<label>x_n: 0.0m </label><br><label>y_n: -1.0m </label><br><label>theta_n: 90.0deg </label><br>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START4'><h3> Start 4 </h3></button><br>");
  
  client.println("<label>x_n: -1.0m </label><br><label>y_n: 0.0m </label><br><label>theta_n: 90.0deg </label><br>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START5'><h3> Start 5 </h3></button><br>");
  
  client.println("<label>x_n: 2.0m </label><br><label>y_n: 2.0m </label><br><label>theta_n: 90.0deg </label><br>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START6'><h3> Start 6 </h3></button><br>");
  
  client.println("<label>x_n: -3.0m </label><br><label>y_n: -3.0m </label><br><label>theta_n: 90.0deg </label><br>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START7'><h3> Start 7 </h3></button><br>");
  
  client.println("<label>x_n: -1.0m </label><br><label>y_n: 3.0m </label><br><label>theta_n: 90.0deg </label><br>");
  client.println("<button style='background-color: #24DB1F; border-radius: 10px; margin:5px' type='button' onClick=location.href='/START8'><h3> Start 8 </h3></button><br>");

  client.println("</div>");
  
  client.println("<button style='background-color: #F53711; border-radius: 10px; margin:5px' type='button' onClick=location.href='/STOP'><h3> Stop </h3></button><br>");
  
  if (command == 0)
    client.println("<label> Current command: </label><label style='background-color: white; padding: 2px'> STOP </label>");
  else
    client.println("<label> Current command: </label><label style='background-color: white; padding: 2px'> WAYPOINT " + String(command) + " </label>");
    
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
