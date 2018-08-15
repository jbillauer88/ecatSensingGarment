#include <Wire.h>
#include <Adafruit_AM2315.h> // Library for external temperature/humidity sensing
//#include <Adafruit_MPL3115A2.h> // Library for altitude/barometric pressure/temperature sensing
//#include "Adafruit_SI1145.h" // Library for UV/Light sensing
#include "Adafruit_SGP30.h" // Library for air quality and gas sensing

Adafruit_AM2315 am2315; // Declaring variable for external temperature/humidity sensor

//Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2(); // Declaring variable for altitude/bp/temp sensor

//Adafruit_SI1145 uv = Adafruit_SI1145(); // Declaring varibale for uv/light sensing

Adafruit_SGP30 sgp; // Declaring variable for air quality/gas sensing

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Starting sensors
  am2315.begin();
  //baro.begin();
  //uv.begin();
  sgp.begin();

  

}

int counter = 0;
void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Hum: "); Serial.println(am2315.readHumidity());
  Serial.print("Temp: "); Serial.println(am2315.readTemperature());

  /*

  float pascals = baro.getPressure();
  // Our weather page presents pressure in Inches (Hg)
  // Use http://www.onlineconversion.com/pressure.htm for other units
  Serial.print(pascals/3377); Serial.println(" Inches (Hg)");

  float altm = baro.getAltitude();
  Serial.print(altm); Serial.println(" meters");

  float tempC = baro.getTemperature();
  Serial.print(tempC); Serial.println("*C");

  

  Serial.print("Vis: "); Serial.println(uv.readVisible());
  Serial.print("IR: "); Serial.println(uv.readIR());

  float UVindex = uv.readUV();
  // the index is multiplied by 100 so to get the
  // integer index, divide by 100!
  UVindex /= 100.0;  
  Serial.print("UV: ");  Serial.println(UVindex);

  */

  sgp.IAQmeasure();

  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

  delay(1000);

  counter++;
  if (counter == 30) {
    counter = 0;

    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      Serial.println("Failed to get baseline readings");
      return;
    }
    Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
    Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
  }

}
