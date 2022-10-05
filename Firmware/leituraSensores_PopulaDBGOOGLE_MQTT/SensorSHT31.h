#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

void initSHT31();

uint32_t getTemperatureSHT31();
uint32_t getHumiditySHT31();
