#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"


void initBME680(void);
void GetGasReference();
String CalculateIAQ(int score);
int GetHumidityScore();
int GetGasScore();
int GetGasScore();
String GetTempBME();
String GetHumBME();
String GetPressBME();
uint16_t BME680IaqSystem();
