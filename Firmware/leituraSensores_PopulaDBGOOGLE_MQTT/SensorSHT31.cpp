#include "SensorSHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

void initSHT31(){
  if (! sht31.begin(0x44)) 
  {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
}

uint32_t getTemperatureSHT31(){
  float t = sht31.readTemperature();
  uint32_t SHT31Response = 0;
  //float h = sht31.readHumidity();
 
  if (! isnan(t)) 
  {
    //Serial.print("Temp *C = "); Serial.println(t);
  } 
  else 
  { 
    Serial.println("Failed to read temperature");
  }
  t = t*100;
  SHT31Response = (uint32_t)t;
  return SHT31Response;
 
  /*if (! isnan(h)) 
  {
    Serial.print("Hum. % = "); Serial.println(h);
  } 
  else 
  { 
    Serial.println("Failed to read humidity");
  }
  Serial.println();*/
}

uint32_t getHumiditySHT31(){
  //float t = sht31.readTemperature();
  uint32_t SHT31Response = 0;
  float h = sht31.readHumidity();
 
  /*if (! isnan(t)) 
  {
    Serial.print("Temp *C = "); Serial.println(t);
  } 
  else 
  { 
    Serial.println("Failed to read temperature");
  }
  t = t*100;
  SHT31Response = (uint32_t)t;*/
 
  if (! isnan(h)) 
  {
    //Serial.print("Hum. % = "); Serial.println(h);
  } 
  else 
  { 
    Serial.println("Failed to read humidity");
  }
  //Serial.println();
  h = h*100;
  SHT31Response = (uint32_t)h;
  return SHT31Response;
}
