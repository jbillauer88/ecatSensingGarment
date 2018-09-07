// Including all sensor libraries along with libraries for I2C

#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_AM2315.h>
#include "Adafruit_SGP30.h"


// Initializing all sensor variables

Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

Adafruit_AM2315 am2315;

Adafruit_SGP30 sgp;

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}


void setup() {
  
  // Starting Serial
  Serial.begin(9600);
    Serial.println("OK!");

  // Starting all sensors
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



}

int counter = 0;
void loop() {
    Serial.println("OK! Loop");

  // Code for MPL3115A2 Sensor ////////////////////////////////////////////////

  Serial.println("===================");
  
  float pascals = baro.getPressure();
  // Our weather page presents pressure in Inches (Hg)
  // Use http://www.onlineconversion.com/pressure.htm for other units
  Serial.print(pascals/3377); 
  //Serial.println(" Inches (Hg)");


  float altm = baro.getAltitude();
  Serial.print(altm); 
  Serial.println(" meters");
 

  float tempC = baro.getTemperature();
  Serial.print(tempC); 
  Serial.println("*C");
  

  //delay(500);

  // Code for AM2315 Sensor ///////////////////////////////////////////////////

  Serial.println("===================");
  
  Serial.print("Hum: "); 
  Serial.println(am2315.readHumidity());
  
  Serial.print("Temp: "); 
  Serial.println(am2315.readTemperature());
  

  //delay(500);

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");
  
  
  delay(50);

}
