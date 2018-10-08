
// M.S.G. Logging Code
// Project ECAT
// By Jeremy Billauer
// October 2018

//////////////////////////////////////////////////////////////


// Including all sensor libraries along with libraries for I2C

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_AM2315.h>
#include "Adafruit_SGP30.h"
#include "Adafruit_VEML6070.h"
#include <Adafruit_GPS.h>

// Initializing SD Variables

File myFile;

const int chipSelect = 4;

// Initializing I2C Sensor Variables

Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2(); // MPL3115A2 Sensor | Altitude + Barometric Pressure

Adafruit_AM2315 am2315; // AM2315 Sensor | Temperature + Humidity

Adafruit_SGP30 sgp; // SGP30 Sensor | Air Quality + Gas

Adafruit_VEML6070 uv = Adafruit_VEML6070(); // VEML6070 Sensor | Light Quality

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

// Initializing GPS Variables

#define GPSSerial Serial1

Adafruit_GPS GPS(&GPSSerial);

#define GPSECHO false

String NMEA1;
String NMEA2;
char c;

// Initializing Interval Variables

uint32_t timer = millis();

int interval = 100;


void setup() {
  // Setup Code

  // Start GPS

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // Request RMC and GGA Sentences Only
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // Request updates on antenna status, comment out to keep quiet
  // GPS.sendCommand(PGCMD_ANTENNA);

  // Starting I2C sensors
  
  startSensors ();

  // Starting SD card

  SD.begin(chipSelect);

}


void loop() {
  // Recurring Code

  // Creating or Opening CSV txt file
  
  myFile = SD.open("sensoria.txt", FILE_WRITE); 

  // Read GPS data

  readGPS();

  if(myFile) {

    // Test if file exists so as only to print header info once per file

    if(myFile.size() < 1){
      
      writeHeader(); 
      
    }

    // Time to separate logging sessions | Creates break everytime program is reset
    
    if(millis() < 3000){
      
      myFile.println(" ");
      
    }

    if (timer > millis()) timer = millis();
     
    // Using interval instead of delay to set resolution of samples
    
    if (millis() - timer > interval) {
      
      timer = millis(); // reset the timer

      if(GPS.fix==1) { //Only save data if we have a fix

        logAlldata();
      
      }    
    }
  }
}

void startSensors () {
  
  baro.begin();
  am2315.begin();
  sgp.begin();
  uv.begin(VEML6070_HALF_T);
  
}

void writeHeader() {

  myFile.print("Day");
  myFile.print(",");
  myFile.print("Time");
  myFile.print(",");
  myFile.print("Latitude");
  myFile.print(",");
  myFile.print("Longitude");
  myFile.print(",");
  myFile.print("Alt (Meters - GPS)");
  myFile.print(",");
  myFile.print("Alt (Meters - Chest)");
  myFile.print(",");
  myFile.print("Baro (Hg)");
  myFile.print(",");
  myFile.print("Temp (In C - Chest)");
  myFile.print(",");
  myFile.print("Temp (In C - Elbow)");
  myFile.print(",");
  myFile.print("Hum %");
  myFile.print(",");
  myFile.print("TVOC (ppb)");
  myFile.print(",");
  myFile.print("EC02 (ppm)");
  myFile.print(",");
  myFile.print("UV (Unitless)");
  myFile.print(",");
  myFile.print("Speed (Knots)");
  myFile.print(",");
  myFile.println("Satellites");
}

void readGPS() {

  clearGPS();
  
  while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
   }
   
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
  NMEA1=GPS.lastNMEA();
  
   while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
  NMEA2=GPS.lastNMEA();

}

void clearGPS() {  //Clear old and corrupt data from serial port 
  
  while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
  
  while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
   while(!GPS.newNMEAreceived()) { //Loop until you have a good NMEA sentence
    c=GPS.read();
  }
  
  GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
  
}

void logI2Cdata () {

  // Code for MPL3115A2 Sensor ///////////////////////////////////////////////////

  float pascals = baro.getPressure();
  myFile.print(pascals/3377); 
  myFile.print(",");  

  float altm = baro.getAltitude();
  myFile.print(altm); 
  myFile.print(",");  
 

  float tempC = baro.getTemperature();
  myFile.print(tempC); 
  myFile.print(",");  

  // Code for AM2315 Sensor ///////////////////////////////////////////////////

  myFile.print(am2315.readTemperature());
  myFile.print(",");  
  
  myFile.print(am2315.readHumidity());
  myFile.print(",");  

  // Code for SGP30 Sensor ///////////////////////////////////////////////////
 
  if (! sgp.IAQmeasure()) {
    //Serial.println("Measurement failed");
    return;
  }
  
  myFile.print(sgp.TVOC);
  myFile.print(",");  
  myFile.print(sgp.eCO2);
  myFile.print(",");  

  // Code for VEML6070 Sensor ////////////////////////////////////////////////
  
  myFile.println(uv.readUV());
  
  myFile.close();
  
}

void logAlldata () {

  // Code for GPS Data - Part 1

  myFile.print(GPS.month, DEC); myFile.print('/');
  myFile.print(GPS.day, DEC); myFile.print('/');
  myFile.print(GPS.year, DEC); 
  myFile.print(",");
  myFile.print(GPS.hour, DEC); myFile.print(':');
  myFile.print(GPS.minute, DEC); myFile.print(':');
  myFile.print(GPS.seconds, DEC); 
  myFile.print(",");
  myFile.print(GPS.latitude,4); // Write measured latitude to file
  myFile.print(GPS.lat); // Which hemisphere N or S
  myFile.print(",");
  myFile.print(GPS.longitude,4); // Write measured longitude to file
  myFile.print(GPS.lon); // Which Hemisphere E or W
  myFile.print(",");
  myFile.print(GPS.altitude);
  myFile.print(",");

  // Code for MPL3115A2 Sensor ///////////////////////////////////////////////////

  float altm = baro.getAltitude();
  myFile.print(altm); 
  myFile.print(",");  
  
  float pascals = baro.getPressure();
  myFile.print(pascals/3377); 
  myFile.print(",");  

  float tempC = baro.getTemperature();
  myFile.print(tempC); 
  myFile.print(",");  

  // Code for AM2315 Sensor ///////////////////////////////////////////////////

  myFile.print(am2315.readTemperature());
  myFile.print(",");  
  
  myFile.print(am2315.readHumidity());
  myFile.print(",");  

  // Code for SGP30 Sensor ///////////////////////////////////////////////////
 
  if (! sgp.IAQmeasure()) {
    //Serial.println("Measurement failed");
    return;
  }
  
  myFile.print(sgp.TVOC);
  myFile.print(",");  
  myFile.print(sgp.eCO2);
  myFile.print(",");  

  // Code for VEML6070 Sensor ////////////////////////////////////////////////
  
  myFile.print(uv.readUV());
  myFile.print(",");

  // Code for GPS Data - Part 2
  
  myFile.print(GPS.speed);
  myFile.print(",");
  myFile.println((int)GPS.satellites);

  // Close the file
  
  myFile.close();
  
}
