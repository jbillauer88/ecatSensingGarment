// Test code for Ultimate GPS Using Hardware Serial (e.g. GPS Flora or FeatherWing)
//
// This code shows how to listen to the GPS module via polling. Best used with
// Feathers or Flora where you have hardware Serial and no interrupt
//
// Tested and works great with the Adafruit GPS FeatherWing
// ------> https://www.adafruit.com/products/3133
// or Flora GPS
// ------> https://www.adafruit.com/products/1059
// but also works with the shield, breakout
// ------> https://www.adafruit.com/products/1272
// ------> https://www.adafruit.com/products/746
// 
// Pick one up today at the Adafruit electronics shop
// and help support open source hardware & software! -ada

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_AM2315.h>
#include "Adafruit_SGP30.h"
#include "Adafruit_VEML6070.h"
#include <Adafruit_GPS.h>

// Declaring variable for datalogging
File myFile;

const int chipSelect = 4;

Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

Adafruit_AM2315 am2315;

Adafruit_SGP30 sgp;

Adafruit_VEML6070 uv = Adafruit_VEML6070();

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
     
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();

uint32_t interval = 10000;


void setup()
{

  SD.begin(chipSelect);
  
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready
  
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  //Serial.begin(9600);
  //Serial.println("Adafruit GPS library basic test!");

  am2315.begin();
  uv.begin(VEML6070_HALF_T);
  baro.begin();
  sgp.begin();
  
     
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
     
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  
  // Ask for firmware version
  //GPSSerial.println(PMTK_Q_RELEASE);
}

void loop() // run over and over again
{

  myFile = SD.open("metadata.txt", FILE_WRITE);
  
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    //if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();
     
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > interval) {
    timer = millis(); // reset the timer
    /*
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    */
    if (GPS.fix) {
      if (myFile) {
        myFile.print("Fix: "); myFile.print((int)GPS.fix);
        myFile.print(","); 
        myFile.print("Location: ");
        myFile.print(GPS.latitude, 4); myFile.print(GPS.lat);
        myFile.print(", ");
        myFile.print(GPS.longitude, 4); myFile.print(GPS.lon);
        myFile.print(","); 
        myFile.print("Speed (knots): "); myFile.print(GPS.speed);
        myFile.print(","); 
        //Serial.print("Angle: "); Serial.println(GPS.angle);
        myFile.print("Altitude: "); myFile.print(GPS.altitude);
        myFile.print(","); 
        myFile.print("Satellites: "); myFile.print((int)GPS.satellites);
        myFile.print(","); 
        //Serial.println("===================");
        myFile.print("Hum: "); myFile.print(am2315.readHumidity());
        myFile.print(","); 
        myFile.print("Temp: "); myFile.print(am2315.readTemperature());
        myFile.print(","); 
        //Serial.println("===================");
        float pascals = baro.getPressure();
        // Our weather page presents pressure in Inches (Hg)
        // Use http://www.onlineconversion.com/pressure.htm for other units
        myFile.print(pascals/3377); 
        //Serial.println(" Inches (Hg)");
        myFile.print(","); 
      
        float altm = baro.getAltitude();
        myFile.print(altm); 
        myFile.print(" meters");
        myFile.print(","); 
      
        float tempC = baro.getTemperature();
        myFile.print(tempC); 
        myFile.print("*C");
        /*Serial.println("===================");
        if (! sgp.IAQmeasure()) {
          Serial.println("Measurement failed");
          return;
        }
        */
        myFile.print(","); 
        myFile.print("TVOC "); myFile.print(sgp.TVOC); myFile.print(" ppb\t");
        myFile.print("eCO2 "); myFile.print(sgp.eCO2); myFile.print(" ppm");
  
        //Serial.println("===================");
        myFile.print(","); 
        myFile.print("UV light level: "); myFile.println(uv.readUV());
        myFile.close();
      }
    }
  }
}
