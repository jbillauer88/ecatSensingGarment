#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_AM2315.h>
#include "Adafruit_SI1145.h"

Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

Adafruit_AM2315 am2315;

Adafruit_SI1145 uv = Adafruit_SI1145();



void setup() {
  
  // Starting Serial
  Serial.begin(9600);

  // Starting all sensors
Serial.print("baro starting.......");
  baro.begin();
Serial.print("am2315 starting.......");
  am2315.begin();
Serial.print("uv starting.......");
  uv.begin();
Serial.println("sensors started");

}

void loop () {

}


