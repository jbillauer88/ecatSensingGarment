#include <Wire.h>
#include <Adafruit_AM2315.h>
#include "Adafruit_SI1145.h"

Adafruit_AM2315 am2315;
Adafruit_SI1145 uv = Adafruit_SI1145();

void setup() {
  Serial.begin(9600);
  

  am2315.begin();
  uv.begin();
  
}

void loop() {
  Serial.println("===================");
  Serial.print("Hum: "); Serial.println(am2315.readHumidity());
  Serial.print("Temp: "); Serial.println(am2315.readTemperature());

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
