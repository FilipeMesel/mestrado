#include "SensorSGP30.h"

SGP30 mySensor; //create an object of the SGP30 class
String SGP30co2, SGP30tvoc = "";

int ledGpio = 2;
void sgp30Init(){
  pinMode(ledGpio,OUTPUT);
  digitalWrite(ledGpio,1);
  Wire.begin();
  uint16_t cmd = 0x2003;
  Wire.beginTransmission(0x58);
  Wire.write(cmd >> 8);
  Wire.write(cmd & 0xFF);
  Wire.endTransmission();
  Wire.begin();
  //Initialize sensor
  if (mySensor.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    int resetBuf[1];
    int idxReset = 0;
    while (1){
        resetBuf[idxReset] = idxReset;
        idxReset++;
        ESP.restart();
    }
  }
  digitalWrite(ledGpio,0);
  //Initializes sensor for air quality readings
  //measureAirQuality should be called in one second increments after a call to initAirQuality
  mySensor.initAirQuality();
  Serial.println("waiting first 20 sensor readings....it's recomended by library");
  for(int i=0;i<20;i++){
    delay(1000); //Wait 1 second
    //measure CO2 and TVOC levels
    mySensor.measureAirQuality();
    Serial.print("desprezou ");
    Serial.print(i);
    Serial.println(" leituras");
  }    
}

void sgp30Readings(){
    mySensor.measureAirQuality();
    SGP30co2 = String(mySensor.CO2);
    SGP30tvoc = String(mySensor.TVOC);
    Serial.print("CO2: ");
    Serial.print(SGP30co2);
    Serial.print(" ppm\tTVOC: ");
    Serial.print(SGP30tvoc);
    Serial.println(" ppb");
}

String sgp30GetCO2(){
    return SGP30co2;
}

String sgp30GetTVOC(){
    return SGP30tvoc;
}
