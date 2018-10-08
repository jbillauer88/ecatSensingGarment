// Including all sensor libraries along with libraries for I2C

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_AM2315.h>
#include "Adafruit_SGP30.h"
#include "Adafruit_VEML6070.h"

// Initializing SD variables

File myFile;

const int chipSelect = 4;

// Initializing all sensor variables

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


void setup() {
  
  // Starting Serial
  //Serial.begin(9600);
  //Serial.println("OK!");

  // Starting all sensors

  uv.begin(VEML6070_HALF_T);
  baro.begin();
  am2315.begin();
  sgp.begin();
  
  // Starting SD card

  SD.begin(chipSelect);
  
  /*
  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    return;
  }

  if (! am2315.begin()) {
     Serial.println("Sensor not found, check wiring & pullups!");
     while (1);
  }


  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
  */


}

int counter = 0;
void loop() {

  myFile = SD.open("sensed.txt", FILE_WRITE | 0_APPEND); // Added Append to just add to previous data

  if (myFile) {

  // Code for MPL3115A2 Sensor ////////////////////////////////////////////////

  //Serial.println("===================");
  
  float pascals = baro.getPressure();
  // Our weather page presents pressure in Inches (Hg)
  // Use http://www.onlineconversion.com/pressure.htm for other units
  myFile.print(pascals/3377); 
  myFile.print(" Inches (Hg)");
  myFile.print(",");  

  float altm = baro.getAltitude();
  myFile.print(altm); 
  myFile.print(" meters");
  myFile.print(",");  
 

  float tempC = baro.getTemperature();
  myFile.print(tempC); 
  myFile.print("*C");
  myFile.print(",");  
  

  //delay(500);

  // Code for AM2315 Sensor ///////////////////////////////////////////////////

  //Serial.println("===================");
  
  myFile.print("Hum: "); 
  myFile.print(am2315.readHumidity());
  myFile.print(",");  
 
  myFile.print("Temp: "); 
  myFile.print(am2315.readTemperature());
  myFile.print(",");  
  

  //delay(500);

  //Serial.println("===================");

  if (! sgp.IAQmeasure()) {
    //Serial.println("Measurement failed");
    return;
  }
  
  myFile.print("TVOC "); myFile.print(sgp.TVOC); myFile.print(" ppb\t");
  myFile.print(",");  
  myFile.print("eCO2 "); myFile.print(sgp.eCO2); myFile.print(" ppm");
  myFile.print(",");  

  // Code for VEML6070 Sensor ////////////////////////////////////////////////

  //Serial.println("===================");
  
  myFile.print("UV light level: "); myFile.println(uv.readUV());
  myFile.close();
  
  

  }

  delay(100);

}
