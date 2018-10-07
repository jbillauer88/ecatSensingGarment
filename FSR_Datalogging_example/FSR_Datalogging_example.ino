/*
 * 2018 Grad Critique Sensor Logger Program
 * 
 * by Jeremy Billauer
 * 
 * Program to log data from two FSR sensors + LSM9DS1 IMU Sensor
 * 
 */



// Including all necessary libraries for sensors and SD logging capability

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  

// i2c
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5

/*
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

// Declaring variable for datalogging
File myFile;

const int chipSelect = 4;

int fsrPin = A0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider

int fsrPin1 = A3;     // the FSR and 10K pulldown are connected to a1
int fsrReading1;     // the analog reading from the FSR resistor divider

int newGround = 5; // the Adalogger pins need reworking to fit with headers
int new3v = 9; //
//int newInput = 9; //


void setupSensor()
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_12GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_16GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
}


void setup() 
{
  pinMode(newGround, OUTPUT);
  pinMode(new3v, OUTPUT);
  //pinMode(newInput, INPUT_PULLUP);
  //pinMode(fsrReading, INPUT_PULLDOWN);
  
  

  digitalWrite(newGround, LOW);
  digitalWrite(new3v, HIGH);
  
  
// Uncomment all serial calls for debugging purposes. They cause issues when not connected to USB.

  
  //Serial.begin(115200);
  
  // Try to initialise and warn if we couldn't detect the chip
  lsm.begin();
  

  SD.begin(chipSelect);

  // helper to just set the default scaling we want, see above!
  setupSensor();
}

void loop() 
{
  /*
  pinMode(fsrPin, OUTPUT);
  digitalWrite(fsrPin, LOW);
  
  pinMode(fsrPin, INPUT);
*/

  fsrReading = analogRead(fsrPin);  
  fsrReading1 = analogRead(fsrPin1);
  
  lsm.read();  /* ask it to read in the data */ 
  

  /* Get a new sensor event */ 
  sensors_event_t a, m, g, temp;

  lsm.getEvent(&a, &m, &g, &temp); 

  
/*
  Serial.print("Gyro X: "); Serial.print(abs(g.gyro.x));   Serial.print(" dps");
  Serial.print("\tY: "); Serial.print(abs(g.gyro.y));      Serial.print(" dps");
  Serial.print("\tZ: "); Serial.print(abs(g.gyro.z));      Serial.println(" dps");

  Serial.println();
  
 
  Serial.print("Left Analog reading = "); Serial.println(fsrReading);   
  Serial.print("Right Analog reading = "); Serial.println(fsrReading1);     // the raw analog reading

  Serial.println();
*/
  

 myFile = SD.open("tyson.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    //Serial.println("Writing to tyson.txt...");
    myFile.print(fsrReading);
    myFile.print(",");   
    //Serial.print("it worked");
    myFile.print(fsrReading1);
    myFile.print(","); 
    myFile.print(abs(g.gyro.x));
    myFile.print(",");  
    myFile.print(abs(g.gyro.y));
    myFile.print(",");  
    myFile.println(abs(g.gyro.z));
    // close the file:
    myFile.close();
    //Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
   //Serial.println("error opening tyson.txt");
  }
 
  
  delay(30000);
  
}
