#include <Arduino.h>
#include "SparkFun_SGP30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include <Wire.h>

void sgp30Init();
void sgp30Readings();
String sgp30GetCO2();
String sgp30GetTVOC();