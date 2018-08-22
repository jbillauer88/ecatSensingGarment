/**************************************************************************/
/*!
    @file     Adafruit_MPL3115A2.cpp
    @author   K.Townsend (Adafruit Industries)
    @license  BSD (see license.txt)

    Example for the MPL3115A2 barometric pressure sensor

    This is a library for the Adafruit MPL3115A2 breakout
    ----> https://www.adafruit.com/products/1893

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0 - First release
*/
/**************************************************************************/

#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include "Adafruit_SI1145.h"

Adafruit_SI1145 uv = Adafruit_SI1145();

// Power by connecting Vin to 3-5V, GND to GND
// Uses I2C - connect SCL to the SCL pin, SDA to SDA pin
// See the Wire tutorial for pinouts for each Arduino
// http://arduino.cc/en/reference/wire
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit_MPL3115A2 test!");

  uv.begin();
  baro.begin();
  
}

void loop() {
  
  
  
  float pascals = baro.getPressure();
  // Our weather page presents pressure in Inches (Hg)
  // Use http://www.onlineconversion.com/pressure.htm for other units
  Serial.print(pascals/3377); Serial.println(" Inches (Hg)");

  float altm = baro.getAltitude();
  Serial.print(altm); Serial.println(" meters");

  float tempC = baro.getTemperature();
  Serial.print(tempC); Serial.println("*C");

  delay(500);

  Serial.println("===================");
  Serial.print("Vis: "); Serial.println(uv.readVisible());
  Serial.print("IR: "); Serial.println(uv.readIR());
  
  // Uncomment if you have an IR LED attached to LED pin!
  //Serial.print("Prox: "); Serial.println(uv.readProx());

  float UVindex = uv.readUV();
  // the index is multiplied by 100 so to get the
  // integer index, divide by 100!
  UVindex /= 100.0;  
  Serial.print("UV: ");  Serial.println(UVindex);

  delay(500);
}
