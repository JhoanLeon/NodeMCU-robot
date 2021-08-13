#include <ESP8266WiFi.h>
#include <Ticker.h>

#define wifi_led D4    // pin on ESP12E board and it's negative
#define stop_led D0    // pin to control through WiFi (as indicator)

String ssid = "CLARO-C84A";
String password = "Cl4r0@F2C84A";

byte command = 0; // byte to code ten commands (1 begin, 8 waypoints, 1 stop) 0 = begin; 1 = waypoint1; 2 = waypoint2; ... 8 = waypoint8; 9 = stop;

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
  digitalWrite(stop_led, LOW);
  
  // determine current command
  switch (current_command) 
  {
    case 0:
      Serial.println("begin command");
    break; 
    
    case 1:
      Serial.println("waypoint 1");
    break;
    
    case 2:
      Serial.println("waypoint 2");
    break;

    case 3:
      Serial.println("waypoint 3");
    break;

    case 4:
      Serial.println("waypoint 4");
    break;

    case 5:
      Serial.println("waypoint 5");
    break;

    case 6:
      Serial.println("waypoint 6");
    break;

    case 7:
      Serial.println("waypoint 7");
    break;

    case 8:
      Serial.println("waypoint 8");
    break;

    case 9:
      Serial.println("stop command");
      digitalWrite(stop_led, HIGH);
    break;
    
    default:
      Serial.println("begin command");
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
  pinMode(stop_led, OUTPUT);
  
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
    digitalWrite(stop_led, HIGH); // turn on stop led
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

  if (request.indexOf("/BEGIN") != -1)
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
  if (request.indexOf("/STOP") != -1)
  {
    command = 9;
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

  client.println("<button style='background-color: #F30700; border-radius: 10px; margin-bottom:10px' type='button' onClick=location.href='/BEGIN'><h3> Set begin </h3></button><br>");
  
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
  
  client.println("<button style='background-color: #F30700; border-radius: 10px; margin:5px' type='button' onClick=location.href='/STOP'><h3> Stop </h3></button><br>");
  
  if (command == 0)
    client.println("<label> Current command: </label><label style='background-color: white; padding: 2px'> BEGIN </label>");
  else if (command == 9)
    client.println("<label> Current command: </label><label style='background-color: white; padding: 2px'> STOP </label>");
  else
    client.println("<label> Current command: </label><label style='background-color: white; padding: 2px'> WAYPOINT " + String(command) + " </label>");
    
  client.println("</div>");

  client.println("<div style='text-align: center; margin: 10px; padding-bottom: 15px; background-color: #19ACEF' class='output'>");
  client.println("<h2>Current Position</h2>");
  client.println("<label>x_i: </label><label style='background-color: white; padding: 2px'> ex. +13.85m </label><br>");
  client.println("<label>y_i: </label><label style='background-color: white; padding: 2px'> ex. -10.26m </label><br>");
  client.println("<label>theta_i: </label><label style='background-color: white; padding: 2px'> ex. +90.03deg </label><br>");
  client.println("<label> </label><br>"); // this tag is empty for space
  
  client.println("<a href='https://imgbb.com/'><img src='https://i.ibb.co/x2kWKYj/top-view-robot.jpg' alt='top-view-robot' border='2'></a>");
  
  client.println("<h2>Current RPMs</h2>");
  client.println("<label>w_1: </label><label style='background-color: white; padding: 2px'> ex. +130rpm </label><br>");
  client.println("<label>w_2: </label><label style='background-color: white; padding: 2px'> ex. -102rpm </label><br>");
  client.println("<label>w_3: </label><label style='background-color: white; padding: 2px'> ex. +220rpm </label><br>");
  client.println("<label>w_4: </label><label style='background-color: white; padding: 2px'> ex. -100rpm </label><br>");
  
  client.println("<h2>Current Distances</h2>");
  client.println("<label>d_1: </label><label style='background-color: white; padding: 2px'> ex. +1.5m </label><br>");
  client.println("<label>d_2: </label><label style='background-color: white; padding: 2px'> ex. +1.4m </label><br>");
  client.println("<label>d_3: </label><label style='background-color: white; padding: 2px'> ex. +2.2m </label><br>");
  client.println("<label>d_4: </label><label style='background-color: white; padding: 2px'> ex. +3.1m </label><br>");
  
  client.println("</div>");
  
  client.println("<div style='text-align: center; margin: 10px; padding-bottom: 15px; background-color: #11C7F5' class='debug'>");
  client.println("<h2>IMU Measurements</h2>");
  client.println("<label>accel_x: </label><label style='background-color: white; padding: 2px'> ex. 1.0g </label><br>");
  client.println("<label>accel_y: </label><label style='background-color: white; padding: 2px'> ex. 2.0g </label><br>");
  client.println("<label>gyro_z: </label><label style='background-color: white; padding: 2px'> ex. 1.3deg/s </label><br>");
 
  client.println("<h2>Behavior</h2>");
  client.println("<label style='background-color: white; padding: 2px'> ex. Go to Goal </label><br>");
  
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
