
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


void setup() {
  // Setup Code

  // Starting I2C sensors
  
  startSensors ();

  // Starting SD card

  SD.begin(chipSelect);

}


void loop() {
  // Recurring Code

  // Creating or Opening CSV txt file
  
  myFile = SD.open("soda.txt", FILE_WRITE); 

  // Insert if(gpsFIX) else (myFile) when GPS logging is figured out

  if(myFile) {

    if(myFile.size() < 1){
      writeHeader();
      myFile.println(" ");
    }
    
    logI2Cdata();
    delay(100);
  }

}

void startSensors () {
  
  baro.begin();
  am2315.begin();
  sgp.begin();
  uv.begin(VEML6070_HALF_T);
  
}

void writeHeader() {

  myFile.print("Baro (Hg)");
  myFile.print(",");
  myFile.print("Alt (Meters)");
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
  myFile.println("UV (Unitless)");
  
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
