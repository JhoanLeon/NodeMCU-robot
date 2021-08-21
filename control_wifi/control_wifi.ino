/*
 * Code made by: Jhoan Esteban León
 * Contact email: je.leon.e@outlook.com
*/

////////////////////////////////////
// Libraries
////////////////////////////////////
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Ticker.h>


////////////////////////////////////
// Constants and Variables 
////////////////////////////////////
#define wifi_led D4    // pin on ESP12E board and it's negative
#define stop_led D0    // pin on board to control through WiFi (as indicator and it's negative)

String ssid = "CLARO-C84A";
String password = "Cl4r0@F2C84A";

byte command = 9; // byte to decode 10 commands (begin, 8 waypoints, stop) 1 = waypoint1; 2 = waypoint2; ... 8 = waypoint8; 9 = stop; 10 = begin; 

WiFiServer server(80);

Ticker tic_wifi_led;

// variables to connect WiFi
byte cont = 0;
byte max_int = 50;

// variables for information from robot
byte data_position[3] = {0}; // [theta_i, y_i, x_i] [2,1,0]

byte data_rpms[4] = {0}; // [w_4, w_3, w_2, w_1] [3,2,1,0]

byte data_distances[4] = {0}; // [d_4, d_3, d_2, d_1] [3,2,1,0]

byte behavior = 0; // byte to decode current robot's behavior

byte data_imu[3] = {0}; // [gyro_z, accel_y, accel_x] [2,1,0] 


////////////////////////////////////
// Auxiliar functions 
////////////////////////////////////
void blink_led() // to indicate while connecting WiFi
{
  byte state = digitalRead(wifi_led);
  digitalWrite(wifi_led, !state);
}


void send_command(byte current_command) // to send command through SPI from WebPage
{
  digitalWrite(stop_led, HIGH); //turn off stop led

  digitalWrite(SS, LOW); // enable condition
  SPI.transfer(current_command); // send through SPI the current command
  digitalWrite(SS, HIGH); // enable release
  
  switch (current_command) // prints on serial console the current command
  {    
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
      digitalWrite(stop_led, LOW); // turn on stop led as indicator
    break;

    case 10:
      Serial.println("begin command");
    break; 
    
    default:
      Serial.println("stop command");
      digitalWrite(stop_led, LOW); // turn on stop led as indicator
    break;
  }
}


void read_info() // to read information of robot from SPI and publish values on WebPage
{  
  // master (NodeMCU) sends p for position, and slave (FPGA) answers with 3 bytes each one for x_i, y_i, theta_i
  digitalWrite(SS, LOW); // enable condition
  SPI.transfer('p'); // send char command to request data
  //SPI.transfer(0x00); // request data from slave
  for (uint8_t i = 0; i < 3; i++) 
  {
    data_position[i] = SPI.transfer(0); // save 3 bytes of info correspondent to x_i, y_i, theta_i
  } 
  digitalWrite(SS, HIGH); // enable release


  // master (NodeMCU) sends r for rpms, and slave (FPGA) answers with 4 bytes each one for each wheel (w_i) 
  digitalWrite(SS, LOW); // enable condition
  SPI.transfer('r'); // send char command to request data
  //SPI.transfer(0x00); // request data from slave
  for (uint8_t i = 0; i < 4; i++) 
  {
    data_rpms[i] = SPI.transfer(0); // save 4 bytes of info correspondent to each wheel w_i
  } 
  digitalWrite(SS, HIGH); // enable release


  // master (NodeMCU) sends d for distances, and slave (FPGA) answers with 4 bytes each one for each proximity sensor (d_i)
  digitalWrite(SS, LOW); // enable condition
  SPI.transfer('d'); // send char command to request data
  //SPI.transfer(0x00); // request data from slave
  for (uint8_t i = 0; i < 4; i++) 
  {
    data_distances[i] = SPI.transfer(0); // save 4 bytes of info correspondent to each proximity sensor d_i
  } 
  digitalWrite(SS, HIGH); // enable release


  // master (NodeMCU) sends b for behavior, and slave (FPGA) answers with 1 byte to represent aproximately 5 behaviors or states
  digitalWrite(SS, LOW); // enable condition
  SPI.transfer('b'); // send char command to request data
  //SPI.transfer(0x00); // request data from slave
  behavior = SPI.transfer(0); // byte to decode robot behaviors
  digitalWrite(SS, HIGH); // enable release

    
  // master (NodeMCU) sends m for imu measurements, and slave (FPGA) answers with 3 bytes each one for each IMU relevant variable (accel_x, accel_y, gyro_z)
  digitalWrite(SS, LOW); // enable condition
  SPI.transfer('m'); // send char command to request data
  //SPI.transfer(0x00); // request data from slave
  for (uint8_t i = 0; i < 3; i++) 
  {
    data_imu[i] = SPI.transfer(0); // save 3 bytes of info correspondent to imu measurements
  } 
  digitalWrite(SS, HIGH); // enable release


  // process information obtained (position needs sign and decimals, rpms needs sign and magnitude, distance needs decimals, behavior just decode number, imu needs sign and decimal)
  Serial.println("read!");
  delay(800);
 
}


////////////////////////////////////
// Main functions 
////////////////////////////////////
void setup() 
{
  pinMode(wifi_led, OUTPUT);
  pinMode(stop_led, OUTPUT);
  
  // Begin serial port with PC
  Serial.begin(115200);
  Serial.println("\n");

  // SPI inicialization with hardware pins (SCLK = D5; MISO = D6; MOSI = D7; SS = D8)
  SPI.begin();
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

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
    digitalWrite(wifi_led, LOW); // turn on wifi led
    Serial.println("*****************************************");
    Serial.print("Connected to WiFi: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC ADRESS: ");
    Serial.println(WiFi.macAddress());
    Serial.println("*****************************************");

    server.begin(); // starts web server
    digitalWrite(stop_led, LOW); // turn on stop led
  }
  else // no connection
  {
    digitalWrite(wifi_led, HIGH); // turn off wifi led
    Serial.println("-----------------------------------------");
    Serial.println("Connection error");
    Serial.println("-----------------------------------------");  
  } 
}


void loop() 
{
  WiFiClient client = server.available(); // search on server for a new client

  read_info(); // read current information of robot from SPI
  
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
    Serial.println("");
    Serial.println("New client...");
    Serial.println(request); // show client's request
  }

  client.flush(); // clear client's request buffer   

  if (request.indexOf("/BEGIN") != -1)
  {
    command = 10;
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

  // web page
  client.println("HTTP/1.1 200 OK");
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
  
  if (command == 10)
    client.println("<label> Current command: </label><label style='background-color: white; padding: 2px'> BEGIN </label>");
  else if (command == 9)
    client.println("<label> Current command: </label><label style='background-color: white; padding: 2px'> STOP </label>");
  else
    client.println("<label> Current command: </label><label style='background-color: white; padding: 2px'> WAYPOINT " + String(command) + " </label>");
    
  client.println("</div>");

  client.println("<div style='text-align: center; margin: 10px; padding-bottom: 15px; background-color: #19ACEF' class='output'>");

  client.println("<button onClick='window.location.reload();'>Refresh Data</button>");
  
  client.println("<h2>Current Position</h2>");
  client.println("<label>x_i: </label><label style='background-color: white; padding: 2px'> " + String(data_position[0]) + "m </label><br>");
  client.println("<label>y_i: </label><label style='background-color: white; padding: 2px'> " + String(data_position[1]) + "m </label><br>");
  client.println("<label>theta_i: </label><label style='background-color: white; padding: 2px'> " + String(data_position[2]) + "deg </label><br>");
  client.println("<label> </label><br>"); // this tag is empty for space organization
  
  client.println("<a href='https://imgbb.com/'><img src='https://i.ibb.co/x2kWKYj/top-view-robot.jpg' alt='top-view-robot' border='2'></a>");
  
  client.println("<h2>Current RPMs</h2>");
  client.println("<label>w_1: </label><label style='background-color: white; padding: 2px'> " + String(data_rpms[0]) + "rpm </label><br>");
  client.println("<label>w_2: </label><label style='background-color: white; padding: 2px'> " + String(data_rpms[1]) + "rpm </label><br>");
  client.println("<label>w_3: </label><label style='background-color: white; padding: 2px'> " + String(data_rpms[2]) + "rpm </label><br>");
  client.println("<label>w_4: </label><label style='background-color: white; padding: 2px'> " + String(data_rpms[3]) + "rpm </label><br>");
  
  client.println("<h2>Current Distances</h2>");
  client.println("<label>d_1: </label><label style='background-color: white; padding: 2px'> " + String(data_distances[0]) + "m </label><br>");
  client.println("<label>d_2: </label><label style='background-color: white; padding: 2px'> " + String(data_distances[1]) + "m </label><br>");
  client.println("<label>d_3: </label><label style='background-color: white; padding: 2px'> " + String(data_distances[2]) + "m </label><br>");
  client.println("<label>d_4: </label><label style='background-color: white; padding: 2px'> " + String(data_distances[3]) + "m </label><br>");

  client.println("<h2>Current Behavior</h2>");
  client.println("<label style='background-color: white; padding: 2px'> " + String(behavior) + " state </label><br>");
  
  client.println("</div>");

  client.println("<div style='text-align: center; margin: 10px; padding-bottom: 15px; background-color: #11C7F5' class='debug'>");
  client.println("<h2>IMU Measurements</h2>");
  client.println("<label>accel_x: </label><label style='background-color: white; padding: 2px'> " + String(data_imu[0]) + "g </label><br>");
  client.println("<label>accel_y: </label><label style='background-color: white; padding: 2px'> " + String(data_imu[1]) + "g </label><br>");
  client.println("<label>gyro_z: </label><label style='background-color: white; padding: 2px'> " + String(data_imu[2]) + "deg/s </label><br>");
 
  client.println("</div>");

  client.println("</body></html>");
  //end of web page
  
  delay(1);
  
  if (request.indexOf("/favicon") == -1) // if favicon doesn't apper on request
  {
    send_command(command); // send current command through SPI
    Serial.println("Request finished");
    Serial.println("");
  }

}
