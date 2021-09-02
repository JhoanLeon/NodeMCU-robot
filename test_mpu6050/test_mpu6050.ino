#include <Wire.h>

const int MPU = 0x68;
float AccX, AccY;
float GyroZ;
float elapsedTime, currentTime, previousTime;

float posX, posY;
float rotation;

float AccErrorX, AccErrorY;
float GyroErrorZ; 
int c = 0;


void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU); // start comunication
  Wire.write(0x6B); // power management register
  Wire.write(0x00);
  Wire.endTransmission(true);

  calculate_IMU_error();
  delay(50);
}


void loop() 
{
  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000; // this is to get time in seconds
  
  // Read accelerometer data
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 4, true);
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X axis, for +- 2g in range then LSB sensitivity parameter 16384.0 
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y axis

  AccX = AccX - 0.03; // adjust with error calculated
  AccY = AccY - 0.02; 

  posX = posX + AccX*9.8*pow(elapsedTime,2); // multiply acceleration on g for m/s^2 and then discrete integrate to obtain position
  posY = posY + AccY*9.8*pow(elapsedTime,2);

  // Read gyroscope data
  Wire.beginTransmission(MPU);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 2, true);
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0; // Z axis, For +- 250 deg/s in range then LSB sensitivity parameter 131.0

  GyroZ = GyroZ + 0.23; // adjust the error with this calculated constant

  // to obtain yaw (Z rotation in deg) it is necessary to discrete integrate the deg/s multiplying this value for time in seconds
  rotation = rotation + GyroZ*elapsedTime;  // deg/s * s = deg 

  // Print value
//  Serial.print("/ Accel X: ");
//  Serial.print(AccX);
  Serial.print("/ Pos X: ");
  Serial.print(posX);
//  Serial.print("/ Accel Y: ");
//  Serial.print(AccY);
  Serial.print("/ Pos Y: ");
  Serial.print(posY);
//  Serial.print("/ Gyro Z: ");
//  Serial.print(GyroZ);
  Serial.print("/ Rotation: ");
  Serial.println(rotation);
  
}


void calculate_IMU_error()
{
  // take 200 measurements of zero value expected, then pound it and get the error
  while(c < 200)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 4, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X axis, for +- 2g in range then LSB sensitivity parameter 16384.0 
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y axis

    // sum all readings
    AccErrorX = AccErrorX + AccX; 
    AccErrorY = AccErrorY + AccY; 
    c++;
  }

  AccErrorX = AccErrorX/200; // divide by 200 to get the error
  AccErrorY = AccErrorY/200; // divide by 200 to get the error
  c = 0;

  // take 200 measurements of zero value expected, then pound it and get the error
  while(c < 200)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x47);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 2, true);
    GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0; // Z axis, For +- 250 deg/s in range then LSB sensitivity parameter 131.0

    // sum all readings
    GyroErrorZ = GyroErrorZ + GyroZ;  
    c++;
  }

  GyroErrorZ = GyroErrorZ/200; // divide by 200 to get the error
  c = 0;

  // print error values
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}

